#ifndef VOXELS_GRAPHICS_ELEMENT_BUFFER_H_
#define VOXELS_GRAPHICS_ELEMENT_BUFFER_H_

#include <cstddef>

namespace voxels::graphics {

class ElementBuffer {
public:
    explicit ElementBuffer(int count, size_t size, const void* data, unsigned int usage);
    ~ElementBuffer();

    ElementBuffer(const ElementBuffer&) = delete;
    ElementBuffer& operator=(const ElementBuffer&) = delete;
    ElementBuffer(ElementBuffer&& other) noexcept;
    ElementBuffer& operator=(ElementBuffer&& other) noexcept;

    void Bind() const noexcept;
    void Unbind() const noexcept;

    int GetCount() const noexcept { return count_; }
private:
    unsigned int id_;
    int count_;
};

} // namespace voxels::graphics

#endif
