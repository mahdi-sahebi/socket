if (BUILD_TESTS)
    set(LIB ${PROJECT_NAME})
    set(TEST test_server)

    find_package(GTest QUIET)
    option(FETCH_GTEST "Download GoogleTest when it is not installed" ON)
    if (NOT GTest_FOUND AND FETCH_GTEST)
        include(FetchContent)
        FetchContent_Declare(
            googletest
            URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.tar.gz
        )
        FetchContent_MakeAvailable(googletest)
    endif()

    if (GTest_FOUND OR TARGET GTest::gtest)
        add_executable(${TEST} test_udp.cc)
        target_link_libraries(${TEST} PRIVATE ${LIB} GTest::gtest)
        add_executable(test_tcp test_tcp.cc)
        target_link_libraries(test_tcp PRIVATE ${LIB} GTest::gtest)
    else()
        message(WARNING "GTest not found; skipping test targets")
    endif()
endif()
