#include "Rf433Controller.h"

#include <iostream>
#include <sstream>

#include <esp_timer.h>

namespace cima::system::network {
    
    const Log Rf433Controller::LOGGER("Rf433Controller");

    const Rf433Controller::Protocol Rf433Controller::proto[] = {
        { 350, {  1, 31 }, {  1,  3 }, {  3,  1 }, false },    // protocol 1
        { 650, {  1, 10 }, {  1,  2 }, {  2,  1 }, false },    // protocol 2
        { 100, { 30, 71 }, {  4, 11 }, {  9,  6 }, false },    // protocol 3
        { 380, {  1,  6 }, {  1,  3 }, {  3,  1 }, false },    // protocol 4
        { 500, {  6, 14 }, {  1,  2 }, {  2,  1 }, false },    // protocol 5
        { 450, { 23,  1 }, {  1,  2 }, {  2,  1 }, true },     // protocol 6 (HT6P20B)
        { 150, {  2, 62 }, {  1,  6 }, {  6,  1 }, false },    // protocol 7 (HS2303-PT, i. e. used in AUKEY Remote)
        { 200, {  3, 130}, {  7, 16 }, {  3,  16}, false},     // protocol 8 Conrad RS-200 RX
        { 200, { 130, 7 }, {  16, 7 }, { 16,  3 }, true},      // protocol 9 Conrad RS-200 TX
        { 365, { 18,  1 }, {  3,  1 }, {  1,  3 }, true },     // protocol 10 (1ByOne Doorbell)
        { 270, { 36,  1 }, {  1,  2 }, {  2,  1 }, true },     // protocol 11 (HT12E)
        { 320, { 36,  1 }, {  1,  2 }, {  2,  1 }, true }      // protocol 12 (SM5212)
    };

    const int Rf433Controller::numProto = sizeof(Rf433Controller::proto) / sizeof(Rf433Controller::proto[0]);

    Rf433Controller::Rf433Controller(gpio_num_t rf433ReceiveGpioPin) 
        : rf433ReceiveGpioPin(rf433ReceiveGpioPin), 
          lastInterruptTime(0), successMessageCounter(0),
          repeatCount(0), changeCount(0),
          nReceivedValue(0), nReceivedBitlength(0),
          nReceivedDelay(0), nReceivedProtocol(0) {}

    void Rf433Controller::initRf433(){
        gpio_config_t io_conf;
        io_conf.intr_type = GPIO_INTR_ANYEDGE;
        io_conf.pin_bit_mask = 1ULL << rf433ReceiveGpioPin;    
        io_conf.mode = GPIO_MODE_INPUT;
        io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
        io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;

        esp_err_t gpioError = gpio_config(&io_conf);
        LOGGER.debug("RF 433 MHz pin result: 0x%x", gpioError);
        
        //TODO ISR service must be extracted from button handler as it is one for all
        esp_err_t handlerError = gpio_isr_handler_add(rf433ReceiveGpioPin, &Rf433Controller::rf433ReceiveHandlerWrapper, this);
        LOGGER.debug("RF 433 MHz handler installation status: 0x%x", handlerError);
        

        LOGGER.debug("RF 433 MHz controller overall fine");
    }

    void Rf433Controller::handleData(){
        
        
        std::queue<Message> newMessages;

        {
            std::lock_guard<std::mutex> guard(messagesMutex);
            messages.swap(newMessages);
        }

        if(newMessages.empty()){
            return;
        }

        LOGGER.info("D: -----------------------");
        while( ! newMessages.empty()){
            std::ostringstream messageDetails;

            auto message = newMessages.front();
            messageDetails << " " << message.nReceivedProtocol << ": " << message.nReceivedValue;
            newMessages.pop();
            
            LOGGER.info("D: %s", messageDetails.str().c_str());
        }
        

        //LOGGER.info("Handling button event from: %s", std::ctime(&eventTime));
        //buttonSignal();
        //messages.pop();
    }

    void Rf433Controller::addReceiveHandler(std::function<void(void)> func){
        buttonSignal.connect(func);
    }

    /* helper function for the receiveProtocol method */
    static inline unsigned int diff(int A, int B) {
        return abs(A - B);
    }

    bool Rf433Controller::receiveProtocol(const int p, unsigned int changeCount) {
        const Protocol &pro = proto[p-1];

        unsigned long code = 0;
        //Assuming the longer pulse length is the pulse captured in timings[0]
        const unsigned int syncLengthInPulses =  ((pro.syncFactor.low) > (pro.syncFactor.high)) ? (pro.syncFactor.low) : (pro.syncFactor.high);
        const unsigned int delay = timings[0] / syncLengthInPulses;
        const unsigned int delayTolerance = delay * N_RECEIVE_TOLERANCE / 100;
        
        /* For protocols that start low, the sync period looks like
        *               _________
        * _____________|         |XXXXXXXXXXXX|
        *
        * |--1st dur--|-2nd dur-|-Start data-|
        *
        * The 3rd saved duration starts the data.
        *
        * For protocols that start high, the sync period looks like
        *
        *  ______________
        * |              |____________|XXXXXXXXXXXXX|
        *
        * |-filtered out-|--1st dur--|--Start data--|
        *
        * The 2nd saved duration starts the data
        */
        const unsigned int firstDataTiming = (pro.invertedSignal) ? (2) : (1);

        for (unsigned int i = firstDataTiming; i < changeCount - 1; i += 2) {
            code <<= 1;
            if (diff(timings[i], delay * pro.zero.high) < delayTolerance &&
                diff(timings[i + 1], delay * pro.zero.low) < delayTolerance) {
                // zero
            } else if (diff(timings[i], delay * pro.one.high) < delayTolerance &&
                    diff(timings[i + 1], delay * pro.one.low) < delayTolerance) {
                // one
                code |= 1;
            } else {
                // Failed
                return false;
            }
        }

        if (changeCount > 7) {    // ignore very short transmissions: no device sends them, so this must be noise
            nReceivedValue = code;
            nReceivedBitlength = (changeCount - 1) / 2;
            nReceivedDelay = delay;
            nReceivedProtocol = p;
            return true;
        }

        return false;
    }

    void Rf433Controller::rf433ReceiveHandler(){
        // ATTENTION: This is IRS call. No monkey bussiness. Just forward over some parralel primitives.

        const long time = esp_timer_get_time();
        const unsigned int duration = time - lastInterruptTime;       

        //--------------------------
        /* below code taken from https://github.com/sui77/rc-switch/blob/master/RCSwitch.cpp */

        if (duration > N_SEPARATION_LIMIT) {
            // A long stretch without signal level change occurred. This could
            // be the gap between two transmission.
            if ((repeatCount==0) || (diff(duration, timings[0]) < 200)) {
                // This long signal is close in length to the long signal which
                // started the previously recorded timings; this suggests that
                // it may indeed by a a gap between two transmissions (we assume
                // here that a sender will send the signal multiple times,
                // with roughly the same gap between them).
                repeatCount++;
                if (repeatCount == 2) {
                    for(unsigned int i = 1; i <= numProto; i++) {
                        if (receiveProtocol(i, changeCount)) {
                            // receive succeeded for protocol i
                            successMessageCounter++;
                            std::lock_guard<std::mutex> guard(messagesMutex);
                            messages.push({
                                nReceivedValue,
                                nReceivedBitlength,
                                nReceivedDelay,
                                nReceivedProtocol
                            });
                            break;
                        }
                    }
                    repeatCount = 0;
                }
            }
            changeCount = 0;
        }
        
        // detect overflow
        if (changeCount >= RCSWITCH_MAX_CHANGES) {
            changeCount = 0;
            repeatCount = 0;
        }

        timings[changeCount++] = duration;
        lastInterruptTime = time; 

    }

    void Rf433Controller::rf433ReceiveHandlerWrapper(void *arg){
        // ATTENTION: This is IRS call. No monkey bussiness. Just forward over some parralel primitives.
        ((Rf433Controller *)arg)->rf433ReceiveHandler();
    }
}