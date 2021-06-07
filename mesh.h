#include <vector>
#include <string>
#include <optional>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

class material;

struct mesh_vertex_t {
	float x, y, z;
	float u, v;
	float nx, ny, nz;
	bool textured, hasNormal;
};

class mesh {
	// Vertex data
	std::vector<mesh_vertex_t> m_vVertexData;

	// Material definitions
	std::vector<material*> m_vMaterials;

	// Texture definitions
	unsigned int m_uTexture;

	// Buffers
	unsigned int VBO;
	unsigned int VAO;
	unsigned int EBO;

	unsigned int NumIndices;
	bool valid = false;

public:
	mesh(const std::vector<mesh_vertex_t> &vertices, const std::vector<unsigned> &indices) {
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

	mesh(unsigned int texture) : m_uTexture{ texture } {};

	void Draw() {
		if (!valid) {
			std::cerr << "Attempted to render invalid mesh: " << std::hex << this << std::endl;
			return;
		}

		// Simply bind VAO and draw
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, NumIndices, GL_UNSIGNED_INT, 0);
	}
};