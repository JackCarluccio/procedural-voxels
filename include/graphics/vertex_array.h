#ifndef VOXELS_GRAPHICS_VERTEX_ARRAY_H_
#define VOXELS_GRAPHICS_VERTEX_ARRAY_H_

namespace voxels::graphics {

class VertexBuffer;

class VertexArray {
public:
    explicit VertexArray();
    ~VertexArray();

    VertexArray(const VertexArray&) = delete;
    VertexArray& operator=(const VertexArray&) = delete;
    VertexArray(VertexArray&& other) noexcept;
    VertexArray& operator=(VertexArray&& other) noexcept;

    void LinkAttribute(const VertexBuffer& vertex_buffer, unsigned int layout, int size, unsigned int type, int stride, const void* offset) const noexcept;
    void LinkAttributeI(const VertexBuffer& vertex_buffer, unsigned int layout, int size, unsigned int type, int stride, const void* offset) const noexcept;
    
    void EnableAttributeLayout(unsigned int layout) const noexcept;
    void DisableAttributeLayout(unsigned int layout) const noexcept;

    void Bind() const noexcept;
    void Unbind() const noexcept;

private:
    unsigned int id_;
};

} // namespace voxels::graphics

#endif // VOXELS_GRAPHICS_VERTEX_ARRAY_H_
