# 项目信息
set(SOURCES
    ${PROJECT_SOURCE_DIR}/source/file.cpp
    ${PROJECT_SOURCE_DIR}/source/main.cpp
)

# 指定生成目标
add_executable(${PROJECT_NAME} ${SOURCES})

# 指定头文件路径
target_include_directories(${PROJECT_NAME}
    PRIVATE 
        ${PROJECT_SOURCE_DIR}/include
)