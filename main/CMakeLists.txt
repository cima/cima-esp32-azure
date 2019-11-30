set(COMPONENT_SRCS
    "CimaEsp32Agent.cpp"
    "Agent.h" "Agent.cpp"
    "system/Log.h" "system/Log.cpp"
    "system/WifiManager.h" "system/WifiManager.cpp"
    "system/WireManager.h" "system/WireManager.cpp"
    "system/EnvironmentSensorManager.h" "system/EnvironmentSensorManager.cpp"
    "system/ManagerNotInitializedException.h"
    "iot/CertSource.cpp" "iot/CertSource.h"
    "iot/IotHubManager.cpp" "iot/IotHubManager.h"
    ${AZURE_IOT_SDK}/certs/certs.c
)

register_component()

enable_language(CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

include_directories(${AZURE_IOT_SDK}/certs)
add_definitions(-DSET_TRUSTED_CERT_IN_SAMPLES)
component_compile_definitions(SET_TRUSTED_CERT_IN_SAMPLES)