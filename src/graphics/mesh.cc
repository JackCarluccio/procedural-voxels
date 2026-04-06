#include "graphics/mesh.h"

#include "graphics/graphics_core.h"
#include "graphics/vertex_array.h"
#include "graphics/vertex_buffer.h"
#include "graphics/element_buffer.h"

namespace voxels::graphics {

Mesh::Mesh(const void* vertices, size_t vertices_size, const void* indices, size_t indices_size, int index_count)
    : vertex_buffer_(vertices_size, vertices, GL_STATIC_DRAW),
    	vertex_array_(),
		element_buffer_(index_count, indices_size, indices, GL_STATIC_DRAW)
{}

void Mesh::LinkAttribute(unsigned int layout, int size, unsigned int type, int stride, const void* offset) const {
	vertex_array_.LinkAttribute(vertex_buffer_, layout, size, type, stride, offset);
	vertex_array_.EnableAttributeLayout(layout);
}

} // namespace voxels::graphics
