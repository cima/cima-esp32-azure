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

register_component()

enable_language(CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_CXX_FLAGS -mlongcalls ${CMAKE_CXX_FLAGS})

component_compile_definitions(SET_TRUSTED_CERT_IN_SAMPLES)
#option(BUILD_SHARED_LIBS OFF)