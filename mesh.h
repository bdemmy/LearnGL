#include <vector>
#include <string>
#include <optional>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>

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

public:
	mesh(const std::string&& path) {
		auto inFile = std::ifstream{ path };
		if (!inFile) {
			return;
		}

		// Face definitions
		std::vector<mesh_vertex_t> vertices{};
		std::vector<unsigned> indices{};

		std::string curLineStr{};
		auto mode = -1;
		while (std::getline(inFile, curLineStr)) {
			if (!strcmp(curLineStr.c_str(), "vertices")) {
				std::cout << "Vertex Mode" << std::endl;
				mode = 0;
				continue;
			}
			if (!strcmp(curLineStr.c_str(), "indices")) {
				std::cout << "Index Mode" << std::endl;
				mode = 1;
				continue;
			}

			std::stringstream stream{ curLineStr };
			if (mode == 0) {
				mesh_vertex_t curVertex{};
				stream >> curVertex.x >> curVertex.y
					>> curVertex.z >> curVertex.u >> curVertex.v
					>> curVertex.nx >> curVertex.ny >> curVertex.nz
					>> curVertex.textured >> curVertex.hasNormal;
				vertices.push_back(curVertex);
			}
			else if (mode == 1) {
				std::string curIndex;
				while (std::getline(stream, curIndex, ' ')) {
					const auto idx = std::stoul(curIndex);
					indices.push_back(idx);
				}
			}
		}

		printf("Num Vertices: %d\nNum Indices: %d\n", vertices.size(), indices.size());
		NumIndices = indices.size();

		//// Generate opengl buffers
		glGenBuffers(1, &VBO); 
		glGenBuffers(1, &EBO);

		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertices[0]), &vertices.front(), GL_STATIC_DRAW);

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

		glVertexAttribIPointer(3, 1, GL_FLOAT, sizeof(mesh_vertex_t), (void*)(8 * sizeof(float)));
		glEnableVertexAttribArray(3);

		glVertexAttribIPointer(4, 1, GL_FLOAT, sizeof(mesh_vertex_t), (void*)(9 * sizeof(float)));
		glEnableVertexAttribArray(4);
	}

	mesh(unsigned int texture) : m_uTexture{ texture } {};

	void Draw() {
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, NumIndices, GL_UNSIGNED_INT, 0);
	}
};