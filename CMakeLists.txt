cmake_minimum_required(VERSION 3.10)
project(TestDessertShop LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ===== 源文件 =====
file(GLOB_RECURSE SOURCES
    "start.cpp"
    "src/*.cpp"
)

# ===== SDL2 路径（相对于项目根目录） =====
set(SDL2_ROOT "${CMAKE_SOURCE_DIR}/SDL2")
set(SDL2_INCLUDE_DIR "${SDL2_ROOT}/include")
set(SDL2_LIB_DIR "${SDL2_ROOT}/lib")

# ===== 头文件目录 =====
include_directories(
    "${CMAKE_SOURCE_DIR}/include"
    "${SDL2_INCLUDE_DIR}"
)

# ===== 库文件目录 =====
link_directories("${SDL2_LIB_DIR}")

# ===== 可执行文件（WIN32 = 无控制台窗口） =====
add_executable(${PROJECT_NAME} WIN32 ${SOURCES})

# ===== 链接库（mingw32 必须在最前面，SDL2main 在 SDL2 之前） =====
target_link_libraries(${PROJECT_NAME}
    mingw32
    SDL2main
    SDL2
    SDL2_ttf
    SDL2_image
    SDL2_mixer
    ole32
    winmm
    imm32
    version
)

# ===== 构建后复制 SDL2 DLL 到输出目录 =====
if(EXISTS "${SDL2_LIB_DIR}/x86/SDL2.dll")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${SDL2_LIB_DIR}/x86/SDL2.dll"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
        COMMENT "复制 SDL2.dll 到输出目录"
    )
endif()
