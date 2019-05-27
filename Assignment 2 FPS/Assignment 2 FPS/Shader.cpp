#include "Shader.h"

// static lite shader manager. 
std::unordered_map<std::string, Shader*> Shader::m_shaders;

bool Shader::LoadShader(std::string ID, LPWSTR vertexShaderPath, LPWSTR pixelShaderPath, DirectX::XMFLOAT3 colour, ID3D11Device* Renderer) {
	if (m_shaders.count(ID) > 0) {
		delete m_shaders[ID];
		m_shaders[ID] = 0;
	}
	m_shaders[ID] = new Shader();
	return m_shaders[ID]->Initialise(Renderer, vertexShaderPath, pixelShaderPath, 0, colour);
}

bool Shader::LoadShader(std::string ID, LPWSTR vertexShaderPath, LPWSTR pixelShaderPath, ID3D11Device* Renderer) {
	if (m_shaders.count(ID) > 0) {
		delete m_shaders[ID];
		m_shaders[ID] = 0;
	}
	m_shaders[ID] = new Shader();
	return m_shaders[ID]->Initialise(Renderer, vertexShaderPath, pixelShaderPath, 1, DirectX::XMFLOAT3(1, 1, 1));
}

Shader* Shader::GetShader(std::string ID) {
	if (m_shaders.count(ID) > 0) {
		return m_shaders[ID];
	}
	else {
		RaiseException(1, 0, 0, 0);
		return NULL;
	}
}


void Shader::Release()
{
	std::unordered_map<std::string, Shader*>::iterator terminator;
	for (terminator = m_shaders.begin(); terminator != m_shaders.end(); terminator++) {
		if (terminator->second) {
			delete terminator->second;
			terminator->second = 0;
		}
	}
}

Shader::Shader()
{
	m_vertexShader = 0;
	m_pixelShader = 0;
	m_layout = 0;
	m_matrixBuffer = 0;
	m_textureSampler = 0;
}


Shader::~Shader()
{
	if (m_matrixBuffer) {
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
	}

	if (m_layout) {
		m_layout->Release();
		m_layout = 0;
	}

	if (m_pixelShader) {
		m_pixelShader->Release();
		m_pixelShader = 0;
	}

	if (m_vertexShader) {
		m_vertexShader->Release();
		m_vertexShader = 0;
	}
	if (m_textureSampler) {
		m_textureSampler->Release();
		m_textureSampler = 0;
	}
}


bool Shader::Initialise(ID3D11Device* device, LPCWSTR vertexFilename, LPCWSTR pixelFilename, bool isTextured, DirectX::XMFLOAT3 colour)
{
	ID3DBlob* vertexShaderBlob = 0;
	ID3DBlob* pixelShaderBlob = 0;
	ID3DBlob* errorBlob = 0;
	const unsigned int numberOfVertexElements = 4;
	D3D11_INPUT_ELEMENT_DESC vertexLayout[numberOfVertexElements];
	D3D11_BUFFER_DESC matrixBufferDescription;

	if (FAILED(D3DCompileFromFile(vertexFilename,
		NULL,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main",
		"vs_4_0",
		D3DCOMPILE_ENABLE_STRICTNESS, 0,
		&vertexShaderBlob, &errorBlob)))
	{
		if (errorBlob)
		{
			OutputShaderErrorMessage(errorBlob, vertexFilename);
			errorBlob->Release();
		}
		if (vertexShaderBlob)
			vertexShaderBlob->Release();

		return false;
	}

	HRESULT result = D3DCompileFromFile(pixelFilename,
		NULL,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main",
		"ps_4_0",
		D3DCOMPILE_ENABLE_STRICTNESS, 0,
		&pixelShaderBlob, &errorBlob);

	if (FAILED(result))
	{
		if (errorBlob)
		{
			OutputShaderErrorMessage(errorBlob, pixelFilename);
			errorBlob->Release();
		}
		if (vertexShaderBlob)
			vertexShaderBlob->Release();

		return false;
	}

	if (FAILED(device->CreateVertexShader(vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), NULL, &m_vertexShader)))
	{
		if (errorBlob)
			errorBlob->Release();

		if (vertexShaderBlob)
			vertexShaderBlob->Release();

		if (pixelShaderBlob)
			pixelShaderBlob->Release();
	}

	if (FAILED(device->CreatePixelShader(pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize(), NULL, &m_pixelShader)))
	{
		if (errorBlob)
			errorBlob->Release();

		if (vertexShaderBlob)
			vertexShaderBlob->Release();

		if (pixelShaderBlob)
			pixelShaderBlob->Release();
	}

	vertexLayout[0].SemanticName = "POSITION";
	vertexLayout[0].SemanticIndex = 0;
	vertexLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexLayout[0].InputSlot = 0;
	vertexLayout[0].AlignedByteOffset = 0;
	vertexLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	vertexLayout[0].InstanceDataStepRate = 0;

	vertexLayout[1].SemanticName = "COLOR";
	vertexLayout[1].SemanticIndex = 0;
	vertexLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	vertexLayout[1].InputSlot = 0;
	vertexLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	vertexLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	vertexLayout[1].InstanceDataStepRate = 0;

	vertexLayout[2].SemanticName = "NORMAL";				//The next element is the normal
	vertexLayout[2].SemanticIndex = 0;
	vertexLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;	//A normal is 3 32-bit floats
	vertexLayout[2].InputSlot = 0;
	vertexLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	vertexLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	vertexLayout[2].InstanceDataStepRate = 0;

	vertexLayout[3].SemanticName = "TEXCOORD";				//The final element is the texture coordinate
	vertexLayout[3].SemanticIndex = 0;
	vertexLayout[3].Format = DXGI_FORMAT_R32G32_FLOAT;		//A texture coord is 2 32-bit floats
	vertexLayout[3].InputSlot = 0;
	vertexLayout[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	vertexLayout[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	vertexLayout[3].InstanceDataStepRate = 0;


	if (FAILED(device->CreateInputLayout(vertexLayout, numberOfVertexElements, vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), &m_layout)))
	{
		if (errorBlob)
			errorBlob->Release();

		if (vertexShaderBlob)
			vertexShaderBlob->Release();

		if (pixelShaderBlob)
			pixelShaderBlob->Release();
	}

	vertexShaderBlob->Release();
	vertexShaderBlob = 0;

	pixelShaderBlob->Release();
	pixelShaderBlob = 0;

	matrixBufferDescription.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDescription.ByteWidth = sizeof(MatrixBuffer);
	matrixBufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDescription.MiscFlags = 0;
	matrixBufferDescription.StructureByteStride = 0;

	if (FAILED(device->CreateBuffer(&matrixBufferDescription, NULL, &m_matrixBuffer)))
	{
		return false;
	}
	if (isTextured) {
		return LoadSampler(device);
	}
	return true;

}


void Shader::Begin(ID3D11DeviceContext* context)
{
	context->IASetInputLayout(m_layout);
	context->VSSetShader(m_vertexShader, NULL, 0);
	context->VSSetConstantBuffers(0, 1, &m_matrixBuffer);
	context->PSSetShader(m_pixelShader, NULL, 0);
	if (m_textureSampler) {
		context->PSSetSamplers(0, 1, &m_textureSampler);
	}
}

bool Shader::SetMatrices(ID3D11DeviceContext* context, DirectX::XMMATRIX worldMatrix, DirectX::XMMATRIX viewMatrix, DirectX::XMMATRIX projectionMatrix)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBuffer* inputData;
	unsigned int bufferNumber;

	if (FAILED(context->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		return false;
	}

	inputData = (MatrixBuffer*)mappedResource.pData;

	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);


	inputData->world = worldMatrix;
	inputData->view = viewMatrix;
	inputData->projection = projectionMatrix;

	context->Unmap(m_matrixBuffer, 0);

	// Set the position of the constant buffer in the vertex shader.
	bufferNumber = 0;

	// Finanly set the constant buffer in the vertex shader with the updated values.
	context->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);
	return true;
}

bool Shader::SetTexture(ID3D11DeviceContext* context, ID3D11ShaderResourceView* textureResource)
{
	if (m_textureSampler) {
		context->PSSetShaderResources(0, 1, &textureResource);
		return true;
	}
	return false;
}

void Shader::OutputShaderErrorMessage(ID3D10Blob* errorMessage, LPCWCHAR shaderFilename)
{
	char* compileErrors;
	unsigned long long bufferSize, i;


	// Get a pointer to the error message text buffer.
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	// Get the length of the message.
	bufferSize = errorMessage->GetBufferSize();


	// Write out the error message.
	for (i = 0; i<bufferSize; i++)
	{
		std::cout << compileErrors[i] << std::endl;
	}

	// Release the error message.
	errorMessage->Release();
	errorMessage = 0;

	// Pop a message up on the screen to notify the user to check the text file for compile errors.
	std::cout << "Error compiling shader. \n";

}

bool Shader::LoadSampler(ID3D11Device* device) {

	D3D11_SAMPLER_DESC textureSamplerDescription;	//When we create a sampler we need a Description struct to describe how we want to create the sampler

	textureSamplerDescription.Filter = D3D11_FILTER_ANISOTROPIC;			//This is the Filtering method used for the texture... 
																			//...different values will give you different quality texture output
	textureSamplerDescription.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;		//This defines what happens when we sample outside of the range [0...1]
	textureSamplerDescription.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;		//In our case we just want it to wrap around so that we always are sampling something
	textureSamplerDescription.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	textureSamplerDescription.MipLODBias = 0.0f;							//The rest of these values are really just "Defaults"
	textureSamplerDescription.MaxAnisotropy = 8;							//If you want more info look up D3D11_SAMPLER_DESC on MSDN
	textureSamplerDescription.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	textureSamplerDescription.BorderColor[0] = 1;
	textureSamplerDescription.BorderColor[1] = 1;
	textureSamplerDescription.BorderColor[2] = 1;
	textureSamplerDescription.BorderColor[3] = 1;
	textureSamplerDescription.MinLOD = 0;
	textureSamplerDescription.MaxLOD = D3D11_FLOAT32_MAX;

	//and create the sampler!
	if (FAILED(device->CreateSamplerState(&textureSamplerDescription, &m_textureSampler)))
	{
		return false;
	}
	return true;
}
