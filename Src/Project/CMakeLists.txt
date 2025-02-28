# 定义 Game 项目
project(MasterComputers)

# 搜索头文件和库路径
set(THIRDPARTY_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/3rdparty/include)
set(THIRDPARTY_LIB_DIR ${CMAKE_SOURCE_DIR}/3rdparty/lib)

# 搜索源文件
file(GLOB_RECURSE ORANGEENGINE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

# 搜索头文件
file(GLOB_RECURSE ORANGEENGINE_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

# 搜索依赖库
find_package(OpenGL REQUIRED)
set(THIRDPARTY 
    glfw3
    glad
    imgui
)

# 添加可执行文件
add_executable(${PROJECT_NAME} ${ORANGEENGINE_SOURCES} ${ORANGEENGINE_HEADERS})

# 链接 OrangeEngine 的头文件和库
target_include_directories(${PROJECT_NAME} PUBLIC
    ${THIRDPARTY_INCLUDE_DIR}
    ${THIRDPARTY_INCLUDE_DIR}/imgui
)
target_link_directories(${PROJECT_NAME} PUBLIC ${THIRDPARTY_LIB_DIR})
target_link_libraries(${PROJECT_NAME}
    ${THIRDPARTY}
)


# 将 Game 设置为 Visual Studio 启动项目
set_target_properties(${PROJECT_NAME} PROPERTIES VS_STARTUP_PROJECT YES)