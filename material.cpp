#include "material.h"

inline glm::vec3 material::get_ambient() const {
	return m_cAmbient;
}

inline glm::vec3 material::get_diffuse() const {
	return m_cDiffuse;
}

inline glm::vec3 material::get_specular() const {
	return m_cSpecular;
}

inline int material::get_illum() const {
	return m_iIllum;
}

inline float material::get_shinyness() const {
	return m_fShiny;
}