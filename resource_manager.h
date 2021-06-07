#include <string>
#include <memory>

class mesh;

namespace resource_manager {
	// Load a texture/image from a file on the system
	// Returns the opengl unsigned int handle to the texture
	// Only supports png and jpg
	unsigned load_texture(const std::string&& path, bool flip_vertically = false);

	// Load a mesh from a file on the system
	// Mainly, load vertices and indices, and pass them to the mesh constructor
	std::unique_ptr<mesh> load_mesh(const std::string&& path);
};