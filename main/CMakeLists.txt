set(COMPONENT_SRCS "CimaEsp32Agent.cpp" "certificates/CertSource.cpp" "certificates/CertSource.h" "log/Log.h" "log/Log.cpp")
set(COMPONENT_ADD_INCLUDEDIRS "." ${AZURE_SDK_PATH_C}/azure-iot-sdk-c/iothub_client)

register_component()

component_compile_definitions(SET_TRUSTED_CERT_IN_SAMPLES)