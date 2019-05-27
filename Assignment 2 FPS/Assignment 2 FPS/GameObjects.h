#ifndef GAMEOBJECTS_H
#define GAMEOBJECTS_H
#include "Direct3D.h"
#include "Mesh.h"
#include "Hex.h"
#include "MathsHelper.h"
// allows us to use the object itself as a key.



class GameObject
{
protected:
	std::string m_name;
	std::vector<Mesh*> m_mesh;
	std::vector<DirectX::XMMATRIX> m_matrix;
	DirectX::XMVECTOR m_position;
	std::vector<DirectX::XMMATRIX> m_offset;
	DirectX::XMVECTOR m_scale;
	DirectX::XMVECTOR m_rotation;
	Shader* m_shader;
	std::vector<Texture*> m_texture;
	float m_movespeed;
	Cell* m_location;

public:
	GameObject(DirectX::XMVECTOR position, std::string shaderID, std::string meshID, std::string textureID = "Grey");
	~GameObject();
	virtual void SetMesh(std::string meshID, unsigned meshIndex = 0);
	void SetTexture(std::string textureID, unsigned textureIndex = 0);
	void setPosition(DirectX::XMVECTOR);
	void SetRotation(DirectX::XMVECTOR);
	void TranslateOffset(DirectX::XMVECTOR translation, unsigned index);
	void Rotate(DirectX::XMVECTOR rotation);
	void Resize(float new_size);
	void Resize(DirectX::XMVECTOR new_size);
	virtual void Update(float timestep);

	// a few quick SAFE conversions to child classes from the parent class, this is to assist the map class in doing fast checks for different item types.
	virtual PhysicsObject* GetInteractive() { return 0; }
	virtual Player* GetActive() { return 0; }
	virtual AIPlayer* GetAI() { return 0; }
	virtual Bullets* GetBullet() { return 0; }
	virtual Loot* GetLoot() { return 0; }
	virtual Player* GetPlayer() { return 0; }


	virtual void Render(Direct3D* renderer, Camera* cam);
	void Move(DirectX::XMVECTOR direction);
	void AddMesh(DirectX::XMVECTOR Offset, DirectX::XMVECTOR Rotation, DirectX::XMVECTOR Scale, std::string meshID, std::string textureID);
	void SetShader(Shader* shader) { m_shader = shader; }
	DirectX::XMVECTOR GetPosition() { return m_position; }
	std::wstring GetPositionString();
	void SetLocation(Cell* location) { m_location = location; }
	// added from base code
	// getters
	float GetXRotation() { DirectX::XMFLOAT3 temp; XMStoreFloat3(&temp, m_rotation); return temp.x; }
	float GetYRotation() { DirectX::XMFLOAT3 temp; XMStoreFloat3(&temp, m_rotation); return temp.y; }
	float GetZRotation() { DirectX::XMFLOAT3 temp; XMStoreFloat3(&temp, m_rotation); return temp.z; }
	float GetXScale() { DirectX::XMFLOAT3 temp; XMStoreFloat3(&temp, m_scale); return temp.x; }
	float GetYScale() { DirectX::XMFLOAT3 temp; XMStoreFloat3(&temp, m_scale); return temp.y; }
	float GetZScale() { DirectX::XMFLOAT3 temp; XMStoreFloat3(&temp, m_scale); return temp.z; }
	Cell* GetLocation() { return m_location; }
	void SetName(std::string name) { m_name = name; }
	// allows implicit conversion from an object to its key.
	operator PointerKey() { return PointerKey{ reinterpret_cast<size_t>(this) }; }
};
#endif