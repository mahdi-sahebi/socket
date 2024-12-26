option(BUILD_TESTS "To add test sources" ON)

if (BUILD_TESTS)
    set(LIB ${PROJECT_NAME})
    set(SERVER_TEST server_test)
    set(CLIENT_TEST client_test)

    find_package(GTest REQUIRED)
    if (NOT GTest_FOUND)
        message(FATAL_ERROR "GTest not found")
    endif()

    # Server
    add_executable(${SERVER_TEST} udp_server.cc)
    target_link_libraries(${SERVER_TEST} PRIVATE ${LIB} GTest::gtest)

    # Client
    add_executable(${CLIENT_TEST} udp_client.cc)
    target_link_libraries(${CLIENT_TEST} PRIVATE ${LIB} GTest::gtest)
endif()
