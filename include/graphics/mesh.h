#pragma once

#include <cstddef>

namespace voxels::graphics {

    class Mesh {
    public:
        template <typename Vertex, typename Index>
        explicit Mesh(const Vertex* vertices, int vertex_count, const Index* indices, int index_count)
            : index_count_(index_count)
        {
            Construct(
                static_cast<const void*>(vertices), sizeof(Vertex) * vertex_count,
                static_cast<const void*>(indices), sizeof(Index) * index_count
            );
        }

        ~Mesh();

        Mesh(const Mesh&) = delete;
        Mesh& operator=(const Mesh&) = delete;
        Mesh(Mesh&& other) noexcept;
        Mesh& operator=(Mesh&& other) noexcept;

        int GetIndexCount() const noexcept { return index_count_; }

        void Bind() noexcept;
        void Unbind() noexcept;

        void LinkAttribute(unsigned int layout, int size, unsigned int type, int stride, const void* offset) noexcept;
        void LinkAttributeI(unsigned int layout, int size, unsigned int type, int stride, const void* offset) noexcept;

    private:
        unsigned int vertex_buffer_id_;
        unsigned int vertex_array_id_;
        unsigned int element_buffer_id_;
        int index_count_;

        void Construct(const void* vertices, size_t vertices_size, const void* indices, size_t indices_size);
    };

}
