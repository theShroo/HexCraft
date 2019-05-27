/*
shader class provided by monash (mike yeates - 2016) and modified extensively by Russell Chidambaranathan 2017
*/

#ifndef SHADER_H
#define SHADER_H

#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <fstream>
#include <iostream>
#include <unordered_map>
#pragma comment(lib, "d3dcompiler.lib")


class Shader
{
private:
	static std::unordered_map<std::string, Shader*> m_shaders;
	struct MatrixBuffer
	{
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX projection;
	};

	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11SamplerState* m_textureSampler;

	void OutputShaderErrorMessage(ID3D10Blob* errorMessage, LPCWCHAR shaderFilename);
	bool Initialise(ID3D11Device* device, LPCWSTR vertexFilename, LPCWSTR pixelFilename, bool isTextured, DirectX::XMFLOAT3 colour);
	bool LoadSampler(ID3D11Device* device);
public:
	static bool LoadShader(std::string ID, LPWSTR vertexShaderPath, LPWSTR pixelShaderPath, DirectX::XMFLOAT3 colour, ID3D11Device* Renderer);
	static bool LoadShader(std::string ID, LPWSTR vertexShaderPath, LPWSTR pixelShaderPath, ID3D11Device* Renderer);

	static Shader* GetShader(std::string ID);
	static void Release();
	Shader();
	virtual ~Shader();

	void Begin(ID3D11DeviceContext* context);
	bool SetMatrices(ID3D11DeviceContext* context, DirectX::XMMATRIX world, DirectX::XMMATRIX view, DirectX::XMMATRIX projection);
	bool SetTexture(ID3D11DeviceContext* context, ID3D11ShaderResourceView* textureView);
};

#endif