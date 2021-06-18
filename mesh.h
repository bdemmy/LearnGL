#ifndef MESH_H
#define MESH_H
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
	int textured, hasNormal;
};

class mesh {
	// Vertex data
	std::vector<mesh_vertex_t> m_vVertexData;

	// Material definitions
	std::vector<material*> m_vMaterials;

	// Texture definitions
	unsigned int m_uTex;
	unsigned int m_uTexNormal;
	unsigned int m_uTexSpecular;

	// Buffers
	unsigned int VBO;
	unsigned int VAO;
	unsigned int EBO;

	unsigned int NumIndices;
	bool valid = false;

public:
	mesh(const std::vector<mesh_vertex_t>& vertices, const std::vector<unsigned>& indices);

	void Draw();
};
#endif // MESH_H