#include <string>
#include <memory>
#include <vector>

class mesh;
class shader;

namespace resource_manager {
	// Load a texture/image from a file on the system
	// Returns the opengl unsigned int handle to the texture
	// Only supports png and jpg
	unsigned load_texture(const std::string path, bool flip_vertically);
	unsigned load_texture(const std::string path);

	// Load a mesh from a file on the system
	// Mainly, load vertices and indices, and pass them to the mesh constructor
	std::shared_ptr<mesh> load_mesh(const std::string path);

	// Load a vertex and fragment shader from a file on the system
	// Returns compiled shader program
	std::shared_ptr<shader> load_shader(std::string shaderName, std::string pathV, std::string pathF);
	std::shared_ptr<shader> load_shader(std::string shaderName);

	// Set the texture/image directory
	void set_texture_directory(std::string&& path);

	// Set the mesh directory
	void set_mesh_directory(std::string&& path);

	// Set the shader directory
	void set_shader_directory(std::string&& path);

	// Load cubemap
	unsigned int load_cubemap(std::vector<std::string> faces);
};