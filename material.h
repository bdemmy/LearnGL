#ifndef MATERIAL_H
#define MATERIAL_H

#include "glm/vec3.hpp"

class material {
	glm::vec3 m_cAmbient {};
	glm::vec3 m_cDiffuse {};
	glm::vec3 m_cSpecular {};
	int       m_iIllum{ 2 };
	float     m_fShiny{ 100 };
	
public:
	material(const glm::vec3 ambient, const glm::vec3 diffuse, const glm::vec3 specular, const int illum, const float shiny) :
		m_cAmbient(ambient), m_cDiffuse(diffuse), m_cSpecular(specular), m_iIllum(illum), m_fShiny(shiny) {}

	[[nodiscard]]
	glm::vec3 get_ambient() const;
	
	[[nodiscard]]
	glm::vec3 get_diffuse() const;
	
	[[nodiscard]]
	glm::vec3 get_specular() const;
	
	[[nodiscard]]
	int get_illum() const;
	
	[[nodiscard]]
	float get_shinyness() const;
};

#endif // MATERIAL_H
