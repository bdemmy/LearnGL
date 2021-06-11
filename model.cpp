#include "model.h"
#include "mesh.h"

glm::mat4 model::get_transform() const {
	auto modelMatrix = glm::mat4(1.0);
	const auto mScale = scale(modelMatrix, m_vScale);
	const auto mRotate = rotate(modelMatrix, glm::radians(m_fYaw), glm::vec3{ 0.f, 1.f, 0.f }) * rotate(modelMatrix, glm::radians(m_fPitch), glm::vec3{ 1.f, 0.f, 0.f });
	const auto mTranslate = translate(modelMatrix, m_vPosition);
	return mTranslate * mRotate * mScale;
}

void model::draw() const {
	m_mShader->use();
	const auto model = get_transform();
	m_mShader->setMatrix("normalModel", glm::inverseTranspose(model));
	m_mShader->setMatrix("model", model);
	m_mShader->setInt("textured", 1);
	m_mShader->setVec3("objectColor", m_vColor);
	m_mMesh->Draw();
}
