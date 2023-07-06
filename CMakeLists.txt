cmake_minimum_required(VERSION 3.11.0)
project(chestnut-ecs VERSION 1.3.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)


# =============================== ALLOW OFFLINE MODE ===================================

FetchContent_Declare(
    FetchContentOffline
    GIT_REPOSITORY https://github.com/SpontanCombust/cmake-fetchcontent-offline
)
set(FETCHCONTENT_UPDATES_DISCONNECTED_FETCHCONTENTOFFLINE ON)
FetchContent_MakeAvailable(FetchContentOffline)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${fetchcontentoffline_SOURCE_DIR}")
include(fetchcontent-offline)

FetchContent_DisconnectedIfOffline()


## ======================================== Options ========================================

option(CHESTNUT_ECS_BUILD_TESTS "Build tests for chestnut-ecs project" OFF)

# set test building on if this is the main project
if (CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    set(CHESTNUT_ECS_BUILD_TESTS ON)
endif()


## ======================================== Library ========================================

set(CTTI_BUILD_TESTS OFF CACHE STRING "")
FetchContent_Declare(
    ctti
    GIT_REPOSITORY https://github.com/Manu343726/ctti
    GIT_TAG 0eb725e531aa3bb2f8222f3cff8fa9ac699a2d3a
)

set(EXPECTED_BUILD_TESTS OFF CACHE STRING "")
FetchContent_Declare(
    expected
    GIT_REPOSITORY https://github.com/TartanLlama/expected
    GIT_TAG v1.1.0
)

set(OPTIONAL_BUILD_TESTS OFF CACHE STRING "")
FetchContent_Declare(
    optional
    GIT_REPOSITORY https://github.com/TartanLlama/optional
    GIT_TAG v1.1.0
)


FetchContent_MakeAvailable(ctti expected optional)


add_library(chestnut_ecs INTERFACE)

target_include_directories(chestnut_ecs INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(chestnut_ecs INTERFACE ctti expected optional)




## ========================================== Tests ========================================

if (CHESTNUT_ECS_BUILD_TESTS)
    FetchContent_Declare(
        catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2
        GIT_TAG v2.13.7
    )

    FetchContent_MakeAvailable(catch2)
    list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/contrib)

    find_package(Threads)



    add_executable(chestnut_ecs_test_auto)
    target_link_libraries(chestnut_ecs_test_auto chestnut_ecs)
    target_link_libraries(chestnut_ecs_test_auto Catch2::Catch2 Threads::Threads)
    target_compile_options(chestnut_ecs_test_auto PRIVATE
        # 4834 - ignored [[nodiscard]] (warning coming from cpp-typelist)
        $<$<CXX_COMPILER_ID:MSVC>: /wd4834 /W4>
    )

    add_executable(chestnut_ecs_test_benchmarks)
    target_link_libraries(chestnut_ecs_test_benchmarks chestnut_ecs)
    target_link_libraries(chestnut_ecs_test_benchmarks Catch2::Catch2 Threads::Threads)
    target_compile_options(chestnut_ecs_test_benchmarks PRIVATE
        # 4834 - ignored [[nodiscard]] (warning coming from cpp-typelist)
        $<$<CXX_COMPILER_ID:MSVC>: /wd4834 /W4>
    )

    add_subdirectory(tests)



    include(CTest)
    include(Catch)
    catch_discover_tests(chestnut_ecs_test_auto)
    catch_discover_tests(chestnut_ecs_test_benchmarks)
endif()
