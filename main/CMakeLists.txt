set(CXX_STANDARD 17)
set(CXX_STANDARD_REQUIRED ON)
set(CXX_EXTENSIONS ON)

set(COMPONENT_SRCS
    "CimaEsp32Agent.cpp"
    "certificates/CertSource.cpp" "certificates/CertSource.h" 
    "system/Log.h" "system/Log.cpp"
    "system/WifiManager.h" "system/WifiManager.cpp"
)

set(COMPONENT_ADD_INCLUDEDIRS "." ${AZURE_SDK_PATH_C}/azure-iot-sdk-c/iothub_client)

register_component()

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
    idf_component_newlib 
    idf_component_ethernet
    ada_bme280

)