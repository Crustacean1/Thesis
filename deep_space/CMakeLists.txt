add_subdirectory(glad/)
include_directories("${PROJECT_SOURCE_DIR}/deep_space/glad/include/")

file(GLOB IMGUI "${PROJECT_SOURCE_DIR}/imgui/*.cpp")
file(GLOB SOURCES **/*.cpp *.cpp)
file(GLOB BACKENDS "${PROJECT_SOURCE_DIR}/imgui/backends/imgui_impl_glfw.cpp"
"${PROJECT_SOURCE_DIR}/imgui/backends/imgui_impl_opengl3.cpp")

#add_library(deep_space_lib ${SOURCES})
add_library(deep_space_lib ${SOURCES} ${IMGUI} ${BACKENDS})

set(OpenGL_GL_PREFERENCE GLVND)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

target_link_libraries(deep_space_lib OpenGL::GL glad)

target_link_libraries(deep_space_lib glfw)
target_include_directories(deep_space_lib PUBLIC "${PROJECT_SOURCE_DIR}/imgui")

configure_file(version.h.in "${PROJECT_SOURCE_DIR}/deep_space/version.h")
