if (BUILD_TESTS)
    set(LIB ${PROJECT_NAME})
    set(TEST test_server)

    find_package(GTest REQUIRED)
    if (NOT GTest_FOUND)
        message(FATAL_ERROR "GTest not found")
    endif()

    add_executable(${TEST} test_udp.cc)
    target_link_libraries(${TEST} PRIVATE ${LIB} GTest::gtest)
    add_executable(test_tcp test_tcp.cc)
    target_link_libraries(test_tcp PRIVATE ${LIB} GTest::gtest)
endif()
