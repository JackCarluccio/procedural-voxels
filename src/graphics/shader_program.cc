#include "graphics/shader_program.h"

#include "graphics/graphics_core.h"

#include <fstream>
#include <stdexcept>

namespace {

std::string ReadFile(const std::filesystem::path& path) {
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file) {
        throw std::runtime_error("Failed to open file: " + path.string());
    }

    std::streamsize size = file.tellg();
    if (size < 0) {
        throw std::runtime_error("Failed to determine file size: " + path.string());
    }

    file.seekg(0, std::ios::beg);

    std::string buffer(static_cast<size_t>(size), '\0');
    if (!file.read(buffer.data(), size)) {
        throw std::runtime_error("Failed to read file: " + path.string());
    }

    return buffer;
}

unsigned int CompileShader(GLenum shader_type, const std::filesystem::path& shader_path) {
    std::string shader_source = ReadFile(shader_path);
    const char* shader_source_cstr = shader_source.c_str();

    // Create and compile shader
    unsigned int id = glCreateShader(shader_type);
    glShaderSource(id, 1, &shader_source_cstr, nullptr);
    glCompileShader(id);

    // Syntax issues could cause compilation to fail
    int success;
    char info_log[512];
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(id, 512, nullptr, info_log);
        throw std::runtime_error("Shader compilation failed (" + shader_path.string() + "): " + info_log);
    }

    return id;
}

} // anonymous namespace

namespace voxels::graphics {

ShaderProgram::ShaderProgram(
    const std::filesystem::path& vertex_shader_path,
    const std::filesystem::path& fragment_shader_path)
    : id_(glCreateProgram())
{
    unsigned int vertex_shader_id = CompileShader(GL_VERTEX_SHADER, vertex_shader_path);
    unsigned int fragment_shader_id = CompileShader(GL_FRAGMENT_SHADER, fragment_shader_path);

    glAttachShader(id_, vertex_shader_id);
    glAttachShader(id_, fragment_shader_id);
    glLinkProgram(id_);

    // Delete shaders as they're linked and no longer necessary
    glDeleteShader(vertex_shader_id);
    glDeleteShader(fragment_shader_id);

    // Linking could fail if shaders are incompatible (different i/o)
    int success;
    char info_log[512];
    glGetProgramiv(id_, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(id_, 512, nullptr, info_log);
        throw std::runtime_error("Shader program linking failed: " + std::string(info_log));
    }
}

ShaderProgram::~ShaderProgram() {
    glDeleteProgram(id_);
}

ShaderProgram::ShaderProgram(ShaderProgram&& other) noexcept : id_(other.id_) {
    other.id_ = 0;
}

ShaderProgram& ShaderProgram::operator=(ShaderProgram&& other) noexcept {
    if (this == &other) {
        return *this;
    }

    glDeleteProgram(id_);
    id_ = other.id_;
    other.id_ = 0;

    return *this;
}

void ShaderProgram::Use() const noexcept {
    glUseProgram(id_);
}

} // namespace voxels::graphics
