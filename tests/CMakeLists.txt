target_sources(${PROJECT_NAME}_Test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/sparse_set_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/entity_signature_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/component_storage_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/entity_registry_test.cpp
    # ${CMAKE_CURRENT_SOURCE_DIR}/entity_query_test.cpp
    # ${CMAKE_CURRENT_SOURCE_DIR}/entity_world_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test_main.cpp
)