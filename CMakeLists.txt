cmake_minimum_required(VERSION 3.20)
project(voxture)
set(CMAKE_CXX_STANDARD 20)

include_directories(external "external/glad/include")

find_package(glfw3 REQUIRED)
add_library(glad "external/glad/include/glad/glad.h" "external/glad/src/glad.c")

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS "src/*.cpp" "src/*.h")
set(TARGET_NAME voxture)
add_executable(${TARGET_NAME} ${SRC_FILES})


set(IMGUI_PATH "external/imgui")

file(GLOB IMGUI_SOURCES ${IMGUI_PATH}/*.cpp)
add_library("ImGui" STATIC ${IMGUI_SOURCES} ${IMGUI_PATH}/backends/imgui_impl_glfw.cpp ${IMGUI_PATH}/backends/imgui_impl_opengl3.cpp)
target_include_directories("ImGui" PUBLIC ${IMGUI_PATH})

target_link_libraries(${TARGET_NAME} PUBLIC ImGui glfw glad ${CMAKE_DL_LIBS})



if(MSVC)
    target_compile_options(${TARGET_NAME} PRIVATE /W4 /WX)
else()
    target_compile_options(${TARGET_NAME} PRIVATE -Wextra -Wpedantic -Werror)
endif()


set(build_resources ${CMAKE_BINARY_DIR}/resources)
set(src_resources ${CMAKE_SOURCE_DIR}/resources)
add_custom_target(link_res COMMAND ${CMAKE_COMMAND} -E create_symlink ${src_resources} ${build_resources})
