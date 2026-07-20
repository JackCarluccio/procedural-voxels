#pragma once

#include <cstddef>

namespace voxels::graphics {

    class Mesh {
    public:
        explicit Mesh(const void* vertices, size_t vertices_size, const void* indices, size_t indices_size, int index_count);

        Mesh(const Mesh&) = delete;
        Mesh& operator=(const Mesh&) = delete;
        Mesh(Mesh&& other) noexcept;
        Mesh& operator=(Mesh&& other) noexcept;

        void Bind() noexcept;
        void Unbind() noexcept;

        void LinkAttribute(unsigned int layout, int size, unsigned int type, int stride, const void* offset) noexcept;
        void LinkAttributeI(unsigned int layout, int size, unsigned int type, int stride, const void* offset) noexcept;

        int GetIndexCount() const noexcept { return index_count_; }

    private:
        unsigned int vertex_buffer_id_;
        unsigned int vertex_array_id_;
        unsigned int element_buffer_id_;
        int index_count_;
    };

}
