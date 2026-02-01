#ifndef VOXELS_GRAPHICS_SHADER_PROGRAM_H_
#define VOXELS_GRAPHICS_SHADER_PROGRAM_H_

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

    void Use() const noexcept;

private:
    unsigned int id_;
};

} // namespace voxels::graphics

#endif // VOXELS_GRAPHICS_SHADER_PROGRAM_H_
