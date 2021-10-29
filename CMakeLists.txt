cmake_minimum_required(VERSION 3.20)
project(voxture)
set(CMAKE_CXX_STANDARD 20)

set(TARGET_NAME voxture)

include_directories(external)
file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS "src/*.cpp")

add_executable(${TARGET_NAME} ${SRC_FILES})

target_link_libraries(${TARGET_NAME} raylib)

if(MSVC)
    target_compile_options(${TARGET_NAME} PRIVATE /W4 /WX)
else()
    target_compile_options(${TARGET_NAME} PRIVATE -Wextra -Wpedantic -Werror)
endif()
