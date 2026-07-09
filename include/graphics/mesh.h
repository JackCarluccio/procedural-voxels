#pragma once

#include "graphics/element_buffer.h"
#include "graphics/vertex_buffer.h"
#include "graphics/vertex_array.h"

#include <cstddef>

#include <glm/glm.hpp>

namespace voxels::graphics {

    class Mesh {
    public:
        explicit Mesh(const void* vertices, size_t vertices_size, const void* indices, size_t indices_size, int index_count);

        Mesh(const Mesh&) = delete;
        Mesh& operator=(const Mesh&) = delete;

        void Bind() noexcept { vertex_array_.Bind(); }
        void Unbind() noexcept { vertex_array_.Unbind(); }

        void LinkAttribute(unsigned int layout, int size, unsigned int type, int stride, const void* offset) noexcept;
        void LinkAttributeI(unsigned int layout, int size, unsigned int type, int stride, const void* offset) noexcept;

        int GetIndexCount() const noexcept { return element_buffer_.GetCount(); }

    private:
        VertexBuffer vertex_buffer_;
        VertexArray vertex_array_;
        ElementBuffer element_buffer_;
    };

}
