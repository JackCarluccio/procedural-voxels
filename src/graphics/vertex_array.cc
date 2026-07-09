#include "graphics/vertex_array.h"

#include "graphics/graphics_core.h"
#include "graphics/vertex_buffer.h"

namespace voxels::graphics {

    VertexArray::VertexArray() {
        glGenVertexArrays(1, &id_);
        glBindVertexArray(id_);
    }

    VertexArray::~VertexArray() {
        glDeleteVertexArrays(1, &id_);
    }

    VertexArray::VertexArray(VertexArray&& other) noexcept : id_(other.id_) {
        other.id_ = 0;
    }

    VertexArray& VertexArray::operator=(VertexArray&& other) noexcept {
        if (this == &other) return *this;

        glDeleteVertexArrays(1, &id_);
        id_ = other.id_;
        other.id_ = 0;

        return *this;
    }

    void VertexArray::LinkAttribute(VertexBuffer& vertex_buffer, unsigned int layout, int size, unsigned int type, int stride, const void* offset) noexcept {
        Bind();
        vertex_buffer.Bind();
        glVertexAttribPointer(layout, size, type, GL_FALSE, stride, offset);
    }

    void VertexArray::LinkAttributeI(VertexBuffer& vertex_buffer, unsigned int layout, int size, unsigned int type, int stride, const void* offset) noexcept {
        Bind();
        vertex_buffer.Bind();
        glVertexAttribIPointer(layout, size, type, stride, offset);
    }

    void VertexArray::EnableAttributeLayout(unsigned int layout) noexcept {
        Bind();
        glEnableVertexAttribArray(layout);
    }

    void VertexArray::DisableAttributeLayout(unsigned int layout) noexcept {
        Bind();
        glDisableVertexAttribArray(layout);
    }

    void VertexArray::Bind() noexcept {
        glBindVertexArray(id_);
    }

    void VertexArray::Unbind() noexcept {
        glBindVertexArray(0);
    }

}
