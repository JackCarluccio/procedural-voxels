#include "graphics/renderer.h"

#include "graphics/graphics_core.h"
#include "graphics/camera.h"
#include "graphics/color.h"
#include "graphics/mesh.h"
#include "graphics/shader_program.h"
#include "util/frustum.h"
#include "world/chunk/chunk.h"

#include <memory>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb_image.h>

namespace {

    unsigned int texture_atlas_id;
    std::unique_ptr<voxels::graphics::ShaderProgram> shader_program;

}

namespace voxels::graphics {

    void Renderer::Init(const application::Settings& settings, unsigned char* texture_data) {
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);

        glEnable(GL_DEPTH_TEST);

        glEnable(GL_FRAMEBUFFER_SRGB);

        if (settings.user_settings.vsync) {
            glfwSwapInterval(0);
        }

        shader_program = std::make_unique<graphics::ShaderProgram>(
            "assets/shaders/chunk_vertex_shader.vert",
            "assets/shaders/chunk_fragment_shader.frag"
        );

        constexpr int DEPTH = 256;
        constexpr int TILE_SIZE = 16;

        // Create texture array to prevent bleeding between tiles when using mipmaps
        GLuint texture_array_id;
        glGenTextures(1, &texture_array_id);
        glBindTexture(GL_TEXTURE_2D_ARRAY, texture_array_id);
        glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_SRGB8_ALPHA8, TILE_SIZE, TILE_SIZE, DEPTH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture_data);

        // Use maximum anisotropic filtering to improve mipmaps at low angles
        GLfloat max_anisotropy;
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &max_anisotropy);
        glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_ANISOTROPY, max_anisotropy);

        // Generate mipmaps to prevent shimmering/sparkling on distant blocks
        glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // Blend pixels when minified
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // Keep pixels crisp when magnified
    }

    void Renderer::Draw(const Scene& scene, const std::unordered_map<glm::ivec2, std::unique_ptr<world::chunk::Chunk>, voxels::util::IVec2Hash>& chunks) {
        Color sky_color = scene.GetSkybox().GetColor();
        glClearColor(sky_color.GetR(), sky_color.GetG(), sky_color.GetB(), sky_color.GetA());

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        const glm::mat4 proj_view = scene.GetCamera().GetProjectionMatrix() * scene.GetCamera().GetViewMatrix();
        shader_program->Use();
        shader_program->SetUniformMatrix4x4("proj_view", glm::value_ptr(proj_view));

        glBindTexture(GL_TEXTURE_2D, texture_atlas_id);

        const util::Frustum frustum = scene.GetCamera().GetFrustum(proj_view);

        for (const auto& [position, chunk] : chunks) {
            auto& chunk_mesh = chunk->GetMesh();
            if (!chunk_mesh) {
                continue;
            }

            const util::AABB aabb = chunk->GetAABB();
            if (!frustum.ContainsAABB(aabb)) {
                continue;
            }
            
            shader_program->SetUniform2i("chunk_world_position", position.x * world::chunk::Chunk::WIDTH, position.y * world::chunk::Chunk::WIDTH);
            
            chunk_mesh->Bind();
            glDrawElements(GL_TRIANGLES, chunk_mesh->GetIndexCount(), GL_UNSIGNED_SHORT, nullptr);
        }

    }

}
