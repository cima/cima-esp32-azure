#pragma once

#include <list>
#include <mutex>
#include <queue>
#include <chrono>
#include <functional>

#include <driver/gpio.h>

#include <boost/signals2/signal.hpp>

#include "../Log.h"

namespace cima::system::network {

    class Rf433Controller {

    public:
        
        struct HighLow {
            uint8_t high;
            uint8_t low;
        };

        /**
         * A "protocol" describes how zero and one bits are encoded into high/low
         * pulses.
         */
        struct Protocol {
            /** base pulse length in microseconds, e.g. 350 */
            uint16_t pulseLength;

            HighLow syncFactor;
            HighLow zero;
            HighLow one;

            /**
             * If true, interchange high and low logic levels in all transmissions.
             *
             * By default, RCSwitch assumes that any signals it sends or receives
             * can be broken down into pulses which start with a high signal level,
             * followed by a a low signal level. This is e.g. the case for the
             * popular PT 2260 encoder chip, and thus many switches out there.
             *
             * But some devices do it the other way around, and start with a low
             * signal level, followed by a high signal level, e.g. the HT6P20B. To
             * accommodate this, one can set invertedSignal to true, which causes
             * RCSwitch to change how it interprets any HighLow struct FOO: It will
             * then assume transmissions start with a low signal lasting
             * FOO.high*pulseLength microseconds, followed by a high signal lasting
             * FOO.low*pulseLength microseconds.
             */
            bool invertedSignal;
        };

        struct Message {
            unsigned long nReceivedValue;
            unsigned int nReceivedBitlength;
            unsigned int nReceivedDelay;
            unsigned int nReceivedProtocol;
        };

        static const Log LOGGER;

        static const unsigned int N_SEPARATION_LIMIT = 4300;
        static const int RCSWITCH_MAX_CHANGES = 67;
        static const int N_RECEIVE_TOLERANCE = 60;

        static const Protocol proto[];
        static const int numProto;

        const gpio_num_t rf433ReceiveGpioPin;
        std::list<std::function<void(void)>> receiveHandlers;

        unsigned long lastInterruptTime;
        unsigned long successMessageCounter;
        unsigned int repeatCount;
        unsigned int changeCount;

        volatile unsigned long nReceivedValue;
        volatile unsigned int nReceivedBitlength;
        volatile unsigned int nReceivedDelay;
        volatile unsigned int nReceivedProtocol;
        
        std::queue<Message> messages;
        std::mutex messagesMutex;

        unsigned int timings[RCSWITCH_MAX_CHANGES];

        // TODO >>>
        
     

        boost::signals2::signal<void ()> buttonSignal;
        boost::signals2::signal<void ()> longButtonSignal;
        //<<< TODO

    public:
        Rf433Controller(gpio_num_t rf433ReceiveGpioPin);
        void initRf433();
        void handleData();

        void addReceiveHandler(std::function<void(void)> func);

    private:
        bool receiveProtocol(const int p, unsigned int changeCount);
        void rf433ReceiveHandler();
        static void rf433ReceiveHandlerWrapper(void *);
    };


}