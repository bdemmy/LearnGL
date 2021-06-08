#include "resource_manager.h"
#include "stb/stb_image.h"
#include "glad/glad.h"
#include <vector>
#include <fstream>
#include "mesh.h"
#include "shader.h"
#include <iostream>

std::string load_file_to_str(const std::string& path) {
	const auto ifs = std::ifstream(path);
	auto sb = std::stringstream{};

	if (ifs) {
		sb << ifs.rdbuf();
		return sb.str();
	}

	return "";
}

namespace resource_manager {
	inline std::string texture_prefix = "textures/";
	inline std::string mesh_prefix = "meshes/";
	inline std::string shader_prefix = "shaders/";

	std::vector<std::shared_ptr<shader>> loaded_shaders;

	unsigned int load_texture(const std::string&& texture, bool flip_vertically) {
		int width, height, channels;

		stbi_set_flip_vertically_on_load(flip_vertically);
		auto* const dat = stbi_load((texture_prefix + texture).c_str(), &width, &height, &channels, 0);

		// Create the texture object, bind it, copy the data, then gen the mipmaps
		unsigned int tex;
		glGenTextures(1, &tex);
		glBindTexture(GL_TEXTURE_2D, tex);

		// Only do the last two if the file exists
		if (dat) {
			// JPG does not use alpha
			if (texture.ends_with("jpg")) {
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, dat);
			}
			else {
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, dat);
			}
			glGenerateMipmap(GL_TEXTURE_2D);
		}

		// Free the texture
		stbi_image_free(dat);

		return tex;
	}

	std::unique_ptr<mesh> load_mesh(const std::string&& path) {
		auto inFile = std::ifstream{ mesh_prefix + path };
		if (!inFile) {
			return nullptr;
		}

		// Store the vertices and indices
		std::vector<mesh_vertex_t> vertices{};
		std::vector<unsigned> indices{};

		// Are we reading vertices, indices, or nothing yet?
		auto mode = -1;
		std::string curLineStr{};
		while (std::getline(inFile, curLineStr)) {
			// Vertex header, start processing vertices
			if (curLineStr.rfind("vertices", 0) == 0) {
				// See if this file header contains the number of vertices total
				// If so, reserve
				int numVerts;
				if (sscanf(curLineStr.c_str(), "vertices %d", &numVerts) == 1) {
					printf("Preallocating for %d vertices.\n", numVerts);
					vertices.reserve(numVerts);
				}

				// Switch to vertex mode
				std::cout << "Vertex Mode" << std::endl;
				mode = 0;
				continue;
			}

			// Index header, start processing indices
			if (curLineStr.rfind("indices", 0) == 0) {
				// Same as above, but for indices
				int nInd;
				if (sscanf(curLineStr.c_str(), "indices %d", &nInd) == 1) {
					printf("Preallocating for %d indices.\n", nInd);
					vertices.reserve(nInd);
				}

				// Index mode
				std::cout << "Index Mode" << std::endl;
				mode = 1;
				continue;
			}

			// Turn the current line into a stream
			// And work upon it accordingly
			std::stringstream stream{ curLineStr };
			if (mode == 0) {
				// If we are in vertex mode
				// Read the line as a vertex into curVertex
				mesh_vertex_t curVertex{};
				stream >> curVertex.x >> curVertex.y
					>> curVertex.z >> curVertex.u >> curVertex.v
					>> curVertex.nx >> curVertex.ny >> curVertex.nz
					>> curVertex.textured >> curVertex.hasNormal;
				// And store that into our vertices
				vertices.push_back(curVertex);
			}
			else if (mode == 1) {
				// Otherwise, keep reading indices until we cant anymore, and append them
				std::string curIndex;
				while (std::getline(stream, curIndex, ' ')) {
					const auto idx = std::stoul(curIndex);
					indices.push_back(idx);
				}
			}
		}

		// Finished reading the file, just some stats
		printf("Num Vertices: %d\nNum Indices: %d\n", vertices.size(), indices.size());
		return std::make_unique<mesh>(vertices, indices);
	}

	std::unique_ptr<shader> load_shader(const std::string&& pathV, const std::string&& pathF) {
		const auto vertex_str = load_file_to_str(shader_prefix + pathV);
		const auto frag_str = load_file_to_str(shader_prefix + pathF);

		if (!vertex_str.empty() && !frag_str.empty()) {
			return std::make_unique<shader>(vertex_str, frag_str);
		}

		return nullptr;
	}

	std::vector<std::shared_ptr<shader>>& get_loaded_shaders() {
		return loaded_shaders;
	}

	void set_texture_directory(std::string&& path) {
		if (!path.ends_with('/') && !path.ends_with('\\')) {
			path += "/";
		}

		texture_prefix = path;
	}

	void set_mesh_directory(std::string&& path) {
		if (!path.ends_with('/') && !path.ends_with('\\')) {
			path += "/";
		}

		mesh_prefix = path;
	}

	void set_shader_directory(std::string&& path) {
		if (!path.ends_with('/') && !path.ends_with('\\')) {
			path += "/";
		}

		shader_prefix = path;
	}
}