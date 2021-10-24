cmake_minimum_required(VERSION 3.0.0)
project(ChestnutECSCore VERSION 0.3.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

## ======================================== ECS Core ========================================

add_library(${PROJECT_NAME} STATIC)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

set_property(TARGET ${PROJECT_NAME} PROPERTY POSITION_INDEPENDENT_CODE ON)

add_subdirectory(src/chestnut/ecs)


install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/ DESTINATION include)
install(TARGETS ${PROJECT_NAME} DESTINATION lib)

## ========================================== Tests ========================================

add_executable(${PROJECT_NAME}_Test)

add_subdirectory(tests)

find_package(Catch2)

target_link_libraries(${PROJECT_NAME}_Test ${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME}_Test Catch2::Catch2)

include(CTest)
include(Catch)
catch_discover_tests(${PROJECT_NAME}_Test)
