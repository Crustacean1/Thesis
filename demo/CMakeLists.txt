
find_package(glm CONFIG REQUIRED)

file(GLOB IMGUI "${PROJECT_SOURCE_DIR}/imgui/*.cpp")
file(GLOB SOURCES *.cpp */*.cpp)
file(GLOB BACKENDS imgui/backends/imgui_impl_glfw.cpp imgui/backends/imgui_impl_opengl3.cpp )

add_executable(deep_space_demo main.cpp ${SOURCES} ${BACKENDS} ${IMGUI})

target_link_libraries(deep_space_demo PUBLIC deep_space_lib)

target_include_directories(deep_space_demo PUBLIC
"${PROJECT_SOURCE_DIR}/deep_space" "${PROJECT_SOURCE_DIR}/imgui")

