#include <string>
#include <memory>

class mesh;
class shader;

namespace resource_manager {
	// Load a texture/image from a file on the system
	// Returns the opengl unsigned int handle to the texture
	// Only supports png and jpg
	unsigned load_texture(const std::string&& path, bool flip_vertically = false);

	// Load a mesh from a file on the system
	// Mainly, load vertices and indices, and pass them to the mesh constructor
	std::unique_ptr<mesh> load_mesh(const std::string&& path);

	// Load a vertex and fragment shader from a file on the system
	// Returns compiled shader program
	std::unique_ptr<shader> load_shader(const std::string&& pathV, const std::string&& pathF);
};