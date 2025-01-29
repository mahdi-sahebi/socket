# Variables
set(PRIVATE_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/include/private/)
set(PUBLIC_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/include/public/)
set(HEADERS
    ${PUBLIC_INCLUDE_DIRS}/socket/socket_interface.h
    ${PUBLIC_INCLUDE_DIRS}/socket/udp_socket_interface.h
    ${PUBLIC_INCLUDE_DIRS}/socket/udp_server.h
    ${PUBLIC_INCLUDE_DIRS}/socket/udp_client.h
)
set(SOURCES socket/udp_server.cc socket/udp_client.cc)
set(LIB ${PROJECT_NAME})


# Executables
add_library(${LIB} STATIC ${SOURCES} ${HEADERS})


# Directories
target_include_directories(${LIB}
    PRIVATE ${PRIVATE_INCLUDE_DIRS}
    PUBLIC  ${PUBLIC_INCLUDE_DIRS}
)


# Linking
