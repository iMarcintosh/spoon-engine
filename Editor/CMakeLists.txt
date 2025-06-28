add_executable(SpoonEditor
    main.cpp
)

target_link_libraries(SpoonEditor 
    PRIVATE 
        SpoonEngine
)

target_include_directories(SpoonEditor 
    PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}
)