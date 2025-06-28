add_executable(BasicExample
    BasicExample/main.cpp
)

target_link_libraries(BasicExample 
    PRIVATE 
        SpoonEngine
)

target_include_directories(BasicExample 
    PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}
)

add_executable(CubeExample
    CubeExample/main.cpp
)

target_link_libraries(CubeExample 
    PRIVATE 
        SpoonEngine
)

target_include_directories(CubeExample 
    PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}
)

add_executable(TexturedCubeExample
    TexturedCubeExample/main.cpp
)

target_link_libraries(TexturedCubeExample 
    PRIVATE 
        SpoonEngine
)

target_include_directories(TexturedCubeExample 
    PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}
)

add_executable(ShadowMappingExample
    ShadowMappingExample/main.cpp
)

target_link_libraries(ShadowMappingExample 
    PRIVATE 
        SpoonEngine
)

target_include_directories(ShadowMappingExample 
    PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}
)

add_executable(ModelLoadingExample
    ModelLoadingExample/main.cpp
)

target_link_libraries(ModelLoadingExample 
    PRIVATE 
        SpoonEngine
)

target_include_directories(ModelLoadingExample 
    PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}
)