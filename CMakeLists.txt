cmake_minimum_required(VERSION 3.20)
project(voxture)
set(CMAKE_CXX_STANDARD 20)

include_directories(external "external/glad/include" "lib/include")

add_library(glad "external/glad/include/glad/glad.h" "external/glad/src/glad.c")
link_directories("lib")
find_package(glfw3 NAMES glfw3)

file(GLOB_RECURSE INIT_EXTERNAL CONFIGURE_DEPENDS "src/init-external/*.cpp")

set(IMGUI_PATH "external/imgui")
file(GLOB IMGUI_SOURCES ${IMGUI_PATH}/*.cpp)
add_library("ImGui" STATIC ${IMGUI_SOURCES} ${IMGUI_PATH}/backends/imgui_impl_glfw.cpp ${IMGUI_PATH}/backends/imgui_impl_opengl3.cpp)
target_include_directories("ImGui" PUBLIC ${IMGUI_PATH})

set(COMMON_DEPS ImGui glfw glad ${CMAKE_DL_LIBS})

add_executable(main src/main.cpp ${INIT_EXTERNAL})
target_link_libraries(main PUBLIC ${COMMON_DEPS})

set(build_resources ${CMAKE_BINARY_DIR}/resources)
set(src_resources ${CMAKE_SOURCE_DIR}/resources)
add_custom_target(link_res COMMAND ${CMAKE_COMMAND} -E create_symlink ${src_resources} ${build_resources})

