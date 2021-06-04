#include "texture.h"
#include "stb/stb_image.h"
#include "glad/glad.h"

unsigned int load_texture(const std::string& texture, bool flip_vertically) {
	int width, height, channels;

	stbi_set_flip_vertically_on_load(flip_vertically);
	auto* const dat = stbi_load(texture.c_str(), &width, &height, &channels, 0);

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