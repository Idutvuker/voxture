cmake_minimum_required(VERSION 3.20)
project(voxelize)
set(CMAKE_CXX_STANDARD 20)

set(TARGET_NAME voxelize)

include_directories(external)
file(GLOB_RECURSE VOXELIZE_SRC CONFIGURE_DEPENDS "src/*.cpp")

add_executable(${TARGET_NAME} ${VOXELIZE_SRC})

if(MSVC)
    target_compile_options(${TARGET_NAME} PRIVATE /W4 /WX)
else()
    target_compile_options(${TARGET_NAME} PRIVATE -Wextra -Wpedantic -Werror)
endif()
