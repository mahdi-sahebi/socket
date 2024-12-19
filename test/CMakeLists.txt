option(BUILD_TESTS "To add test sources" ON)

if (BUILD_TESTS)
    set(LIB ${PROJECT_NAME})

    add_executable(server udp_server.cc)
    target_link_libraries(server PRIVATE ${LIB})

    add_executable(client udp_client.cc)
    target_link_libraries(client PRIVATE ${LIB})
endif()
