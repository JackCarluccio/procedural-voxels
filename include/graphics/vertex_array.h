#pragma once

namespace voxels::graphics {

    class VertexBuffer;

    class VertexArray {
    public:
        VertexArray();
        ~VertexArray();

        VertexArray(const VertexArray&) = delete;
        VertexArray& operator=(const VertexArray&) = delete;
        VertexArray(VertexArray&& other) noexcept;
        VertexArray& operator=(VertexArray&& other) noexcept;

        void LinkAttribute(VertexBuffer& vertex_buffer, unsigned int layout, int size, unsigned int type, int stride, const void* offset) noexcept;
        void LinkAttributeI(VertexBuffer& vertex_buffer, unsigned int layout, int size, unsigned int type, int stride, const void* offset) noexcept;
        
        void EnableAttributeLayout(unsigned int layout) noexcept;
        void DisableAttributeLayout(unsigned int layout) noexcept;

        void Bind() noexcept;
        void Unbind() noexcept;

    private:
        unsigned int id_;
    };

}
