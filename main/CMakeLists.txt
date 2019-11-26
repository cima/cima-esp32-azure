set(COMPONENT_SRCS
    "CimaEsp32Agent.cpp"
    "system/Log.h" "system/Log.cpp"
    "system/WifiManager.h" "system/WifiManager.cpp"
    "system/WireManager.h" "system/WireManager.cpp"
    "system/EnvironmentSensorManager.h" "system/EnvironmentSensorManager.cpp"
    "system/ManagerNotInitializedException.h"
    "iot/CertSource.cpp" "iot/CertSource.h"
    "iot/IotHubManager.cpp" "iot/IotHubManager.h"
)

set(COMPONENT_ADD_INCLUDEDIRS "." ${AZURE_SDK_PATH_C}/azure-iot-sdk-c/iothub_client)

register_component()

enable_language(CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)
CONFIG_COMPILER_CXX_EXCEPTIONS
set(CXX_FLAGS ${CXX_FLAGS} -std=c++17 -Wfatal-errors)

component_compile_definitions(SET_TRUSTED_CERT_IN_SAMPLES)
option(BUILD_SHARED_LIBS OFF)

set (EXTRA_COMPONENT_DIRS $ENV{IDF_PATH}/components ${AZURE_SDK_PATH_C}/port)

target_link_libraries(${COMPONENT_TARGET} 
    idf_component_esp32
    idf_component_soc
    idf_component_heap 
    idf_component_driver 
    idf_component_log 
    idf_component_freertos 
    idf_component_esp_event 
    idf_component_tcpip_adapter 
    idf_component_lwip 
    idf_component_vfs 
    idf_component_ethernet
    ada_bme280

)