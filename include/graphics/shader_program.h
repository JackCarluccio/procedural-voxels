#pragma once

#include <filesystem>
#include <string>

namespace voxels::graphics {

    class ShaderProgram {
    public:
        explicit ShaderProgram(
            const std::filesystem::path& vertex_shader_path,
            const std::filesystem::path& fragment_shader_path
        );
        ~ShaderProgram();

        ShaderProgram(const ShaderProgram&) = delete;
        ShaderProgram& operator=(const ShaderProgram&) = delete;
        ShaderProgram(ShaderProgram&& other) noexcept;
        ShaderProgram& operator=(ShaderProgram&& other) noexcept;

        void Use() noexcept;
        void SetUniform2i(const std::string& name, int v0, int v1) noexcept;
        void SetUniformMatrix4x4(const std::string& name, const float* value) noexcept;

    private:
        unsigned int id_;
    };

}
