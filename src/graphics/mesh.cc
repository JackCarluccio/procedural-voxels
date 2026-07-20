#include "graphics/mesh.h"

#include "graphics/graphics_core.h"

namespace voxels::graphics {

	Mesh::Mesh(const void* vertices, size_t vertices_size, const void* indices, size_t indices_size, int index_count)
		: index_count_(index_count)
	{
		glGenBuffers(1, &vertex_buffer_id_);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id_);
        glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertices_size), vertices, GL_STATIC_DRAW);

		glGenVertexArrays(1, &vertex_array_id_);
        glBindVertexArray(vertex_array_id_);

		glGenBuffers(1, &element_buffer_id_);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_id_);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(indices_size), indices, GL_STATIC_DRAW);
	}

	Mesh::Mesh(Mesh&& other) noexcept
		: vertex_buffer_id_(other.vertex_buffer_id_),
		  vertex_array_id_(other.vertex_array_id_),
		  element_buffer_id_(other.element_buffer_id_),
		  index_count_(other.index_count_)
	{
		other.vertex_buffer_id_ = 0;
		other.vertex_array_id_ = 0;
		other.element_buffer_id_ = 0;
		other.index_count_ = 0;
	}

	Mesh& Mesh::operator=(Mesh&& other) noexcept {
		if (this == &other) return *this;

		glDeleteBuffers(1, &vertex_buffer_id_);
		glDeleteVertexArrays(1, &vertex_array_id_);
		glDeleteBuffers(1, &element_buffer_id_);

		vertex_buffer_id_ = other.vertex_buffer_id_;
		vertex_array_id_ = other.vertex_array_id_;
		element_buffer_id_ = other.element_buffer_id_;
		index_count_ = other.index_count_;

		other.vertex_buffer_id_ = 0;
		other.vertex_array_id_ = 0;
		other.element_buffer_id_ = 0;
		other.index_count_ = 0;

		return *this;
	}

	void Mesh::Bind() noexcept {
		glBindVertexArray(vertex_array_id_);
	}

	void Mesh::Unbind() noexcept {
		glBindVertexArray(0);
	}

	void Mesh::LinkAttribute(unsigned int layout, int size, unsigned int type, int stride, const void* offset) noexcept {
		glBindVertexArray(vertex_array_id_);
		glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id_);
		glVertexAttribPointer(layout, size, type, GL_FALSE, stride, offset);
		glEnableVertexAttribArray(layout);
	}

	void Mesh::LinkAttributeI(unsigned int layout, int size, unsigned int type, int stride, const void* offset) noexcept {
		glBindVertexArray(vertex_array_id_);
		glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id_);
		glVertexAttribIPointer(layout, size, type, stride, offset);
		glEnableVertexAttribArray(layout);
	}

}
