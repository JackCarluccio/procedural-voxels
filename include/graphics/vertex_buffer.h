#ifndef VOXELS_GRAPHICS_VERTEX_BUFFER_H_
#define VOXELS_GRAPHICS_VERTEX_BUFFER_H_

#include <cstddef>

namespace voxels::graphics {

class VertexBuffer {
public:
    explicit VertexBuffer(size_t size, const void* data, unsigned int usage);
    ~VertexBuffer();

    VertexBuffer(const VertexBuffer&) = delete;
    VertexBuffer& operator=(const VertexBuffer&) = delete;
    VertexBuffer(VertexBuffer&& other) noexcept;
    VertexBuffer& operator=(VertexBuffer&& other) noexcept;

    void Bind() const noexcept;
    void Unbind() const noexcept;

private:
    unsigned int id_;
};

} // namespace voxels::graphics

#endif // VOXELS_GRAPHICS_VERTEX_BUFFER_H_
