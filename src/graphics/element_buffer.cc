#include "graphics/element_buffer.h"

#include "graphics/graphics_core.h"

namespace voxels::graphics {

ElementBuffer::ElementBuffer(int count, size_t size, const void* data, unsigned int usage) : count_(count) {
    glGenBuffers(1, &id_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(size), data, usage);
    Unbind();
}

ElementBuffer::~ElementBuffer() {
    glDeleteBuffers(1, &id_);
}

ElementBuffer::ElementBuffer(ElementBuffer&& other) noexcept : id_(other.id_), count_(other.count_) {
    other.id_ = 0;
    other.count_ = 0;
}

ElementBuffer& ElementBuffer::operator=(ElementBuffer&& other) noexcept {
    if (this == &other) return *this;

    glDeleteBuffers(1, &id_);
    id_ = other.id_;
    other.id_ = 0;
    count_ = other.count_;
    other.count_ = 0;
    
    return *this;
}

void ElementBuffer::Bind() const noexcept {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_);
}

void ElementBuffer::Unbind() const noexcept {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

} // namespace voxels::graphics
