# Variables
set(ARCH "UNKNOWN_PLATFORM")
set(DEPS "")
if (UNIX)
    set(ARCH unix)
elseif (WIN32)
    set(ARCH windows)
    set(DEPS Ws2_32)
endif()
set(PRIVATE_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/include/private/)
set(PUBLIC_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/include/public/)
set(HEADERS
    ${PUBLIC_INCLUDE_DIRS}/socket/socket_interface.h
    ${PUBLIC_INCLUDE_DIRS}/socket/udp_socket_interface.h
    ${PUBLIC_INCLUDE_DIRS}/socket/udp_server.h
    ${PUBLIC_INCLUDE_DIRS}/socket/udp_client.h
)
set(SOURCES socket/${ARCH}/udp_server.cc socket/${ARCH}/udp_client.cc)
set(LIB ${PROJECT_NAME})


# Executables
add_library(${LIB} STATIC ${SOURCES} ${HEADERS})


# Directories
target_include_directories(${LIB}
    PRIVATE ${PRIVATE_INCLUDE_DIRS}
    PUBLIC  ${PUBLIC_INCLUDE_DIRS}
)


# Linking
target_link_libraries(${LIB}
    PRIVATE ${DEPS}
)