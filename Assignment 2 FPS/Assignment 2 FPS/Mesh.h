#ifndef MESH_H
#define MESH_H

#include "Direct3D.h"
#include "Camera.h"
#include "Shader.h"
#include "Texture.h"
#include <string>

class Mesh {
	
	struct Vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT4 colour;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT2 texCoord;
	};
private:
	int m_vertexCount;
	int m_indexCount;
	int m_referenceCount;
	D3D11_PRIMITIVE_TOPOLOGY m_topology;
	ID3D11Buffer* m_vertexBuffer;
	ID3D11Buffer* m_indexBuffer;

	DirectX::XMVECTOR m_top;
	DirectX::XMVECTOR m_bottom;
	DirectX::XMVECTOR m_minVector;	//For our bounding boxes we need to know the min position of the mesh...
	DirectX::XMVECTOR m_maxVector;	//... and the max position
	DirectX::XMVECTOR m_centre;		//For the bounding spheres we need to know the centre point...
	float m_radius;			//... and the overall radius.

	Mesh();
	Mesh(Direct3D* direct3D, std::string filepath);
	~Mesh();

	bool InitialiseBuffers(Direct3D* renderer, Vertex* vertexData, unsigned long* indexData);


public:
	static std::unordered_map<std::string, Mesh*> m_meshes;

	static void Release();
	static Mesh* GetMesh(std::string meshid);
	static void LoadMesh(std::string meshid, std::string filepath, Direct3D* renderer);

	int GetIndexCount() { return m_indexCount; }
	int GetVertexCount() { return m_vertexCount; }
	void Render(Direct3D* direct3D, DirectX::XMMATRIX world, Camera* cam, Shader* shader, Texture* texture);
	DirectX::XMVECTOR GetMin() { return m_minVector; }
	DirectX::XMVECTOR GetMax() { return m_maxVector; }
	DirectX::XMVECTOR GetCentre() { return m_centre; }
	DirectX::XMVECTOR GetTop() { return m_top; }
	DirectX::XMVECTOR GetBottom() { return m_bottom; }

	float GetRadius() { return m_radius; }

};

#endif