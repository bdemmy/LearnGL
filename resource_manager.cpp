#include "resource_manager.h"
#include "stb/stb_image.h"
#include "glad/glad.h"
#include <vector>
#include <fstream>
#include "mesh.h"
#include "shader.h"
#include <iostream>
#include <unordered_map>
#include <tuple>
#include "tuplehash.h"

using namespace std;

unordered_map<string, shared_ptr<shader>> mp_loadedShaders;
unordered_map<string, shared_ptr<mesh>> mp_loadedMeshes;
unordered_map<tuple<string, bool>, unsigned int> mp_loadedTextures;

std::string load_file_to_str(const std::string& path) {
	const auto ifs = std::ifstream(path);

	if (ifs) {
		auto buffer = std::stringstream{};
		buffer << ifs.rdbuf();
		return buffer.str();
	}

	return "";
}

namespace resource_manager {
	inline std::string texture_prefix = "textures/";
	inline std::string mesh_prefix = "meshes/";
	inline std::string shader_prefix = "shaders/";

	unsigned int load_texture(const std::string texture, bool flip_vertically) {
		const auto result = mp_loadedTextures.find({ texture, flip_vertically });
		if (result != mp_loadedTextures.end()) {
			return result->second;
		}
		
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

		mp_loadedTextures[{ texture, flip_vertically }] = tex;
		return tex;
	}

	unsigned int load_texture(const std::string texture) {
		return load_texture(texture, false);
	}
	
	std::shared_ptr<mesh> load_mesh(const std::string path) {
		const auto completePath = mesh_prefix + path;

		const auto result = mp_loadedMeshes.find(completePath);
		if (result != mp_loadedMeshes.end()) {
			return result->second;
		}
		
		auto inFile = std::ifstream{ completePath };
		
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

		return (mp_loadedMeshes[completePath] = make_shared<mesh>(vertices, indices));
	}

	std::shared_ptr<shader> load_shader(const std::string name, const std::string pathV, const std::string pathF) {
		const auto totalPathV = shader_prefix + pathV;
		const auto totalPathF = shader_prefix + pathF;

		const auto result = mp_loadedShaders.find(name);
		if (result != mp_loadedShaders.end()) {
			return result->second;
		}

		const auto vertex_str = load_file_to_str(totalPathV);
		const auto frag_str = load_file_to_str(totalPathF);

		if (!vertex_str.empty() && !frag_str.empty()) {
			return (mp_loadedShaders[name] = std::make_shared<shader>(vertex_str, frag_str));
		}

		return nullptr;
	}
	
	std::shared_ptr<shader> load_shader(const std::string name) {
		const auto totalPathV = shader_prefix + name + ".vert";
		const auto totalPathF = shader_prefix + name + ".frag";

		return load_shader(name, totalPathV, totalPathF);
	}

	unordered_map<tuple<string, bool>, unsigned int>& get_loaded_shaders() {
		return mp_loadedTextures;
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

	unsigned int load_cubemap(std::vector<std::string> faces)
	{
		unsigned int textureID;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

		int width, height, nrChannels;
		for (unsigned int i = 0; i < faces.size(); i++)
		{
			unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
			if (data)
			{
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
					0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
				);
				stbi_image_free(data);
			}
			else
			{
				std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
				stbi_image_free(data);
			}
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		return textureID;
	}
}