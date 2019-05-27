#include "GameObjects.h"
using namespace DirectX;

GameObject::GameObject(XMVECTOR position, std::string shaderID, std::string meshID, std::string textureID) {
	m_mesh.push_back(Mesh::GetMesh(meshID));
	m_matrix.push_back(XMMatrixIdentity());
	m_offset.push_back(XMMatrixIdentity());
	m_position = position;
	m_scale = MathsHelper::GetXMVECTOR3(1.0f, 1.0f, 1.0f);
	m_rotation = MathsHelper::GetXMVECTOR3(0.0f, 1.5708f, 0.0f);
	m_texture.push_back(Texture::GetTexture(textureID));
	m_shader = Shader::GetShader(shaderID);
	m_movespeed = 1.5f;

}
GameObject::~GameObject() {
}

void GameObject::SetMesh(std::string meshID, unsigned meshIndex) {
	if (meshIndex <= m_mesh.size()) {
		m_mesh[meshIndex] = Mesh::GetMesh(meshID);
	}
}

void GameObject::SetTexture(std::string textureID, unsigned textureIndex) {
	if (textureIndex <= m_texture.size()) {
		m_texture[textureIndex] = Texture::GetTexture(textureID);
	}


}
void GameObject::AddMesh(XMVECTOR offset, XMVECTOR rotation, XMVECTOR scale, std::string meshID, std::string textureID) {
	m_mesh.push_back(Mesh::GetMesh(meshID));
	m_matrix.push_back(XMMatrixIdentity());
	m_offset.push_back(XMMatrixRotationRollPitchYawFromVector(rotation) * XMMatrixScalingFromVector(scale) * XMMatrixTranslationFromVector(offset));
	m_texture.push_back(Texture::GetTexture(textureID));


}

void GameObject::setPosition(XMVECTOR new_position) {
	m_position = new_position;
}

void GameObject::Update(float timestep) {
	for (int i = 0; i < m_matrix.size(); i++) {
		m_matrix[i] = m_offset[i] * XMMatrixRotationRollPitchYawFromVector(m_rotation) * XMMatrixScalingFromVector(m_scale) * XMMatrixTranslationFromVector(m_position);
	}
}

void GameObject::Render(Direct3D* renderer, Camera* cam) {
	for (int i = 0; i < m_mesh.size(); i++) {
		if (m_texture[i]) {
			m_mesh[i]->Render(renderer, m_matrix[i], cam, m_shader, m_texture[i]);
		}
	}
}

void GameObject::Resize(float new_size) {
	m_scale = m_scale*new_size;
}

void GameObject::Resize(XMVECTOR new_size) {
	m_scale = new_size;
}


void GameObject::Move(XMVECTOR direction) {
	m_position += direction;
}

void GameObject::SetRotation(XMVECTOR rotation) {
	m_rotation = rotation;
}

void GameObject::Rotate(XMVECTOR rotation) {
	// unsure on this logic, it may be very wrong.
	m_rotation = m_rotation + rotation;
}


std::wstring GameObject::GetPositionString() {
	XMFLOAT3 pos;
	XMStoreFloat3(&pos, m_position);
	std::wstringstream ss;
	ss << "x:" << pos.x << ", y:" << pos.y << ", z:" << pos.z;
	return ss.str();
}

void GameObject::TranslateOffset(XMVECTOR translation, unsigned index) {
	m_offset[index] *= XMMatrixTranslationFromVector(translation);
}
