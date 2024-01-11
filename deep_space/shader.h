#ifndef SHADER
#define SHADER

#include <cstdio>
#include <exception>
#include <filesystem>
#include <functional>
#include <glm/mat4x4.hpp>
#include <memory>
#include <optional>

#include "shader_builder.h"

class ShaderException : public std::exception {
    char message[512];

  public:
    ShaderException(const char *message) {
        snprintf(this->message, 512, "shader error: %s", message);
    };

    virtual const char *what() const throw() { return message; }
};

class Shader {
  public:
    Shader();
    Shader(unsigned int program);
    Shader(const Shader &shader);
    ~Shader();

    const unsigned int program;

    int uniform(const char *name);

    static Shader load(std::vector<std::string> files,
                       std::function<std::vector<unsigned char>(const std::string &)>);

    static Shader default_resource();

    static void load(int uniform, glm::mat4);
    static void load(int uniform, glm::vec3);
    static void load(int uniform, float);
    static void load(int uniform, unsigned int);
};

#endif /*SHADER*/
