#include "graphics/vertex_array.h"

#include "graphics/graphics_core.h"
#include "graphics/vertex_buffer.h"

namespace voxels::graphics {

VertexArray::VertexArray() {
    glGenVertexArrays(1, &vertex_array_id_);
}

VertexArray::~VertexArray() {
    glDeleteVertexArrays(1, &vertex_array_id_);
}

VertexArray::VertexArray(VertexArray&& other) noexcept
    : vertex_array_id_(other.vertex_array_id_) {
    other.vertex_array_id_ = 0;
}

VertexArray& VertexArray::operator=(VertexArray&& other) noexcept {
    if (this == &other) return *this;

    glDeleteVertexArrays(1, &vertex_array_id_);
    
    vertex_array_id_ = other.vertex_array_id_;
    other.vertex_array_id_ = 0;

    return *this;
}

void VertexArray::LinkAttribute(const VertexBuffer& vertex_buffer, unsigned int layout, int size, unsigned int type, int stride, const void* offset) const noexcept {
    Bind();
    vertex_buffer.Bind();
    glVertexAttribPointer(layout, size, type, GL_FALSE, stride, offset);
    vertex_buffer.Unbind();
    Unbind();
}

void VertexArray::LinkAttributeI(const VertexBuffer& vertex_buffer, unsigned int layout, int size, unsigned int type, int stride, const void* offset) const noexcept {
    Bind();
    vertex_buffer.Bind();
    glVertexAttribIPointer(layout, size, type, stride, offset);
    vertex_buffer.Unbind();
    Unbind();
}

void VertexArray::EnableAttributeLayout(unsigned int layout) const noexcept {
    Bind();
    glEnableVertexAttribArray(layout);
    Unbind();
}

void VertexArray::DisableAttributeLayout(unsigned int layout) const noexcept {
    Bind();
    glDisableVertexAttribArray(layout);
    Unbind();
}

void VertexArray::Bind() const noexcept {
    glBindVertexArray(vertex_array_id_);
}

void VertexArray::Unbind() const noexcept {
    glBindVertexArray(0);
}

} // namespace voxels::graphics
