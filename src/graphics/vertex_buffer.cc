#include "graphics/vertex_buffer.h"

#include "graphics/graphics_core.h"

namespace voxels::graphics {

VertexBuffer::VertexBuffer(size_t size, const void* data, unsigned int usage) {
    glGenBuffers(1, &vertex_buffer_id_);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id_);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(size), data, usage);

    // Unbind to force explicit binding later
    Unbind();
}

VertexBuffer::~VertexBuffer() {
    glDeleteBuffers(1, &vertex_buffer_id_);
}

VertexBuffer::VertexBuffer(VertexBuffer&& other) noexcept
    : vertex_buffer_id_(other.vertex_buffer_id_)
{
    other.vertex_buffer_id_ = 0;
}

VertexBuffer& VertexBuffer::operator=(VertexBuffer&& other) noexcept {
    if (this == &other) return *this;

    glDeleteBuffers(1, &vertex_buffer_id_);

    vertex_buffer_id_ = other.vertex_buffer_id_;
    other.vertex_buffer_id_ = 0;

    return *this;
}

void VertexBuffer::Bind() const noexcept {
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id_);
}

void VertexBuffer::Unbind() const noexcept {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

} // namespace voxels::graphics
