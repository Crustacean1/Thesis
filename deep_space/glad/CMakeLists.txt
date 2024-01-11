add_library(glad "${PROJECT_SOURCE_DIR}/deep_space/glad/include/glad/glad.h" "${PROJECT_SOURCE_DIR}/deep_space/glad/src/glad.cpp")

target_include_directories(glad PUBLIC ./include/)

