#include "NetworkManager.h"

namespace cima::system::network {

    NetworkManager::NetworkManager() : connected(false) {

    }

    void NetworkManager::registerNetworkUpHandler(std::function<void(void)> func) {
        networkUpSignal.connect(func);
    }

    void NetworkManager::registerNetworkDownHandler(std::function<void(void)> func){
        networkDownSignal.connect(func);
    }

}