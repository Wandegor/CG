#pragma once
#include "pch.h"
#include "Vertex.h"

class Mesh
{
public:
	using Vertices = std::vector<Vertex>;
	using Indices = std::vector<unsigned>;

	enum class PrimitiveType
	{
		Triangles,
		Quads,
	};

	Mesh(Vertices vertices, Indices indices, PrimitiveType primitiveType)
		: m_vertices(std::move(vertices))
		, m_indices(std::move(indices))
		, m_primitiveType(primitiveType)
	{
	}

	void Draw() const
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		glBegin(GetGLPrimitiveType(m_primitiveType));
		for (auto index : m_indices)
		{
			glColor4ubv(glm::value_ptr(m_vertices[index].color));
			glVertex3fv(glm::value_ptr(m_vertices[index].position));
		}
		glEnd();
	}

	void DrawWithArrays() const
	{
		if (m_vertices.empty() || m_indices.empty())
			return;

		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, sizeof(Vertex),
			glm::value_ptr(m_vertices.data()->position));

		glEnableClientState(GL_COLOR_ARRAY);
		glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vertex),
			glm::value_ptr(m_vertices.data()->color));

		glDrawElements(GetGLPrimitiveType(m_primitiveType),
			static_cast<GLsizei>(m_indices.size()), GL_UNSIGNED_INT, m_indices.data());

		glDisableClientState(GL_COLOR_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);
	}

private:
	Vertices m_vertices;
	Indices m_indices;
	PrimitiveType m_primitiveType;

	GLenum GetGLPrimitiveType(Mesh::PrimitiveType type) const
	{
		switch (type)
		{
			case Mesh::PrimitiveType::Triangles:
				return GL_TRIANGLES;
			case Mesh::PrimitiveType::Quads:
				return GL_QUADS;
		}
		return GL_POINTS;
	}
};
