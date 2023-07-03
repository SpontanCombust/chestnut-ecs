target_sources(chestnut_ecs_test_auto PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/test_main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sparse_set_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/entity_signature_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/component_storage_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/entity_registry_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/entity_world_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/entity_world_querying_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/commands_test.cpp
)

target_sources(chestnut_ecs_test_benchmarks PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/test_main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sparse_set_benchmarks.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/entity_world_benchmarks.cpp
)