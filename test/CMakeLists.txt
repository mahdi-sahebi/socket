option(BUILD_TESTS "To add test sources" ON)

if (BUILD_TESTS)
    set(LIB ${PROJECT_NAME})
    set(TEST test_server)

    find_package(GTest REQUIRED)
    if (NOT GTest_FOUND)
        message(FATAL_ERROR "GTest not found")
    endif()

    add_executable(${TEST} test_udp.cc)
    target_link_libraries(${TEST} PRIVATE ${LIB} GTest::gtest)
endif()
