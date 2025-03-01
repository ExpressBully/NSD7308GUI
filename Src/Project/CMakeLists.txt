# 定义 Game 项目
project(MasterComputers)

# 搜索头文件和库路径
set(THIRDPARTY_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/3rdparty/include)
set(THIRDPARTY_LIB_DIR ${CMAKE_SOURCE_DIR}/3rdparty/lib)
# 设置资源文件的源目录和目标目录
set(RESOURCE_SOURCE_DIR ${CMAKE_SOURCE_DIR}/Resource)
set(RESOURCE_OUTPUT_DIR ${CMAKE_BINARY_DIR}/Resource)

# 搜索源文件
file(GLOB_RECURSE MSC_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

# 搜索头文件
file(GLOB_RECURSE MSC_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

# 搜索依赖库
find_package(OpenGL REQUIRED)
set(THIRDPARTY 
    glfw3
    glad
    imgui
)

# 添加可执行文件
add_executable(${PROJECT_NAME} ${MSC_SOURCES} ${MSC_HEADERS})

# 链接 MSC 的头文件和库
target_include_directories(${PROJECT_NAME} PUBLIC
    ${THIRDPARTY_INCLUDE_DIR}
    ${THIRDPARTY_INCLUDE_DIR}/imgui
)
target_link_directories(${PROJECT_NAME} PUBLIC ${THIRDPARTY_LIB_DIR})

set(ALL_SOURCE_FILE
    ${MSC_HEADERS}
    ${MSC_SOURCES}
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}
    FILES ${ALL_SOURCE_FILE}
)

target_link_libraries(${PROJECT_NAME}
    ${THIRDPARTY}
)

# 添加自定义命令以复制资源文件
add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${RESOURCE_SOURCE_DIR} ${RESOURCE_OUTPUT_DIR}
)

# 将 MasterComputers 设置为 Visual Studio 启动项目
set_target_properties(${PROJECT_NAME} PROPERTIES VS_STARTUP_PROJECT YES)