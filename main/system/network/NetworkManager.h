#pragma once

#include <string>
#include <list>
#include <iterator>

#include <esp_event_base.h>
#include <boost/signals2/signal.hpp>

#include "../Log.h"


namespace cima::system::network {

class NetworkManager {

    static Log LOG;

protected:    
    boost::signals2::signal<void ()> networkUpSignal;
    boost::signals2::signal<void ()> networkDownSignal;

    bool connected;
    //bool started; //TODO Do we really need this?

public:
    NetworkManager();
    virtual bool isConnected() = 0;

    virtual void registerNetworkUpHandler(std::function<void(void)> func);
    virtual void registerNetworkDownHandler(std::function<void(void)> func);    

    //void start(); //TODO Do we really need this?
    //bool isStarted();//TODO Do we really need this?
};

}