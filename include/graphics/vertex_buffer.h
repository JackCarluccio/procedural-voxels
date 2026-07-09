#pragma once

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

        void Bind() noexcept;
        void Unbind() noexcept;

    private:
        unsigned int id_;
    };

}
