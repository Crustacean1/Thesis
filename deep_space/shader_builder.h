#ifndef SHADER_BUILDER
#define SHADER_BUILDER

#include <filesystem>
#include <functional>
#include <map>
#include <optional>
#include <string>

class Shader;

class ShaderBuilder {
  public:
    ShaderBuilder();
    enum ShaderType { Vertex, Fragment, Geometry };
    ShaderBuilder compile(ShaderType type, const std::vector<unsigned char> & source);
    unsigned int build();

    static int to_gl(ShaderType type);
    static const char *filename(ShaderType type);

  private:
    int program;
};

#endif /*SHADER_BUILDER*/
