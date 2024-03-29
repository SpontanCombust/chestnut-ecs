cmake_minimum_required(VERSION 3.0.0)
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


## ======================================== ECS Core ========================================

FetchContent_Declare(
    typelist
    GIT_REPOSITORY https://github.com/SpontanCombust/cpp-typelist
    GIT_TAG master
)

FetchContent_MakeAvailable(typelist)


add_library(${PROJECT_NAME} INTERFACE)

target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME} INTERFACE typelist)




## ========================================== Tests ========================================

FetchContent_Declare(
    catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2
    GIT_TAG v2.13.7
)

FetchContent_MakeAvailable(catch2)
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/contrib)

find_package(Threads)



add_executable(${PROJECT_NAME}_Test)

add_subdirectory(tests)

target_link_libraries(${PROJECT_NAME}_Test ${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME}_Test Catch2::Catch2 Threads::Threads)

target_compile_options(${PROJECT_NAME}_Test PRIVATE
    # 4834 - ignored [[nodiscard]] (warning coming from cpp-typelist)
    $<$<CXX_COMPILER_ID:MSVC>: /wd4834 /W4>
)



include(CTest)
include(Catch)
catch_discover_tests(${PROJECT_NAME}_Test)
