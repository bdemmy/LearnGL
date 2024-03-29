#include "mesh.h"
#include "glad/glad.h"

mesh::mesh(const std::vector<mesh_vertex_t>& vertices, const std::vector<unsigned>& indices) {
	NumIndices = indices.size();

	// Generate opengl buffers
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	// Generate our VAO
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// Copy our vertex data into vbo
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertices[0]), &vertices.front(), GL_STATIC_DRAW);

	// Copy our indices into our ebo
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(indices[0]), &indices.front(), GL_STATIC_DRAW);

	// X,Y,Z
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(mesh_vertex_t), nullptr);
	glEnableVertexAttribArray(0);

	// U,V,W
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(mesh_vertex_t), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// Normal X,Y,Z 
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(mesh_vertex_t), (void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// Textured?
	glVertexAttribIPointer(3, 1, GL_FLOAT, sizeof(mesh_vertex_t), (void*)(8 * sizeof(float)));
	glEnableVertexAttribArray(3);

	// Normals?
	glVertexAttribIPointer(4, 1, GL_FLOAT, sizeof(mesh_vertex_t), (void*)(9 * sizeof(float)));
	glEnableVertexAttribArray(4);

	valid = true;
}

void mesh::Draw() {
	if (!valid) {
		std::cerr << "Attempted to render invalid mesh: " << std::hex << this << std::endl;
		return;
	}

	// Simply bind VAO and draw
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, NumIndices, GL_UNSIGNED_INT, 0);
}