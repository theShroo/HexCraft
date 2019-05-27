#include "Texture.h"
#include "DirectXTK/WICTextureLoader.h"
#include <sstream>

std::unordered_map<std::string, Texture*> Texture::textures;


void Texture::Release() {
	std::unordered_map<std::string, Texture*>::iterator terminator;
	for (terminator = textures.begin(); terminator != textures.end(); terminator++) {
		if (terminator->second) {
			delete terminator->second;
			terminator->second = 0;
		}
	}
	textures.clear();
}

Texture* Texture::GetTexture(std::string textureID) {
	if (textures.count(textureID) > 0) {
		return textures[textureID];
	}
	else {
		RaiseException(2, 0, 0, 0);
		return NULL;

	}

}

Texture::~Texture() {
	Terminate();

}

void Texture::LoadTexture(Direct3D* renderer, std::string textureID, char* filename) {
	if (textures.count(textureID) > 0) {
		delete textures[textureID];
	}
	if (filename != "Null") {
		textures[textureID] = new Texture(renderer, filename);
	}
	else
		textures[textureID] = 0;
}

Texture::Texture(Direct3D* renderer, char* filename) {
	Initialise(renderer, filename);
}

bool Texture::Initialise(Direct3D* renderer, char* filename) {

	
	// additional texture file type supported: Targa.
	if (LoadTarga(filename, renderer)) {
		return true;
	}
	else {
		wchar_t* wideFilename = MathsHelper::ConvertString(filename);

		HRESULT result = DirectX::CreateWICTextureFromFile(renderer->GetDevice(), wideFilename, &m_texture, &m_textureView);

		if (FAILED(result))
		{
			return false;
		}

		return true;
	}
	std::cout << "failed to load image file\n";
	return false;
}



void Texture::Terminate() {

	// Release the texture view resource.
	if (m_textureView)
	{
		m_textureView->Release();
		m_textureView = 0;
	}

	// Release the texture.
	if (m_texture)
	{
		m_texture->Release();
		m_texture = 0;
	}

	// Release the targa data.

}

bool Texture::LoadTarga(char* filename, Direct3D* renderer) {
	unsigned char* targaData;
	int height, width;
	int error, bpp, imageSize, index, i, j, k;
	FILE* filePtr;
	unsigned int count;
	TargaHeader targaFileHeader;
	unsigned char* targaImage;

	// replacing the strangly un-cooperative WICTextureLoader with a simple targa loader.
	
	ID3D11DeviceContext* deviceContext = renderer->GetDeviceContext();
	ID3D11Device* device = renderer->GetDevice();
	
	D3D11_TEXTURE2D_DESC textureDesc;

	unsigned int rowPitch;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;

	// Open the targa file for reading in binary.
	error = fopen_s(&filePtr, filename, "rb");
	if (error != 0)
	{
		std::cout << "failed to open image file\n" << filename << "\n";
		return false;
	}
	// Read in the file header.
	count = (unsigned int)fread(&targaFileHeader, sizeof(TargaHeader), 1, filePtr);
	if (count != 1)
	{
		return false;
	}

	// Get the important information from the header.
	height = (int)targaFileHeader.height;
	width = (int)targaFileHeader.width;
	bpp = (int)targaFileHeader.bpp;

	// Check that it is 32 bit and not 24 bit.
	if (bpp != 32)
	{
		return false;
	}

	// Calculate the size of the 32 bit image data.
	imageSize = width * height * 4;

	// Allocate memory for the targa image data.
	targaImage = new unsigned char[imageSize];
	if (!targaImage)
	{
		return false;
	}

	// Read in the targa image data.
	count = (unsigned int)fread(targaImage, 1, imageSize, filePtr);
	if (count != imageSize)
	{
		return false;
	}

	// Close the file.
	error = fclose(filePtr);
	if (error != 0)
	{
		return false;
	}

	// Allocate memory for the targa destination data.
	targaData = new unsigned char[imageSize];
	if (!targaData)
	{
		return false;
	}

	// Initialize the index into the targa destination data array.
	index = 0;

	// Initialize the index into the targa image data.
	k = (width * height * 4) - (width * 4);

	// Now copy the targa image data into the targa destination array in the correct order since the targa format is stored upside down.
	for (j = 0; j < height; j++)
	{
		for (i = 0; i < width; i++)
		{
			targaData[index + 0] = targaImage[k + 2];  // Red.
			targaData[index + 1] = targaImage[k + 1];  // Green.
			targaData[index + 2] = targaImage[k + 0];  // Blue
			targaData[index + 3] = targaImage[k + 3];  // Alpha
					
			// Increment the indexes into the targa data.
			k += 4;
			index += 4;
		}

		// Set the targa image data index back to the preceding row at the beginning of the column since its reading it in upside down.
		k -= (width * 8);
	}

	// Release the targa image data now that it was copied into the destination array.
	delete[] targaImage;
	targaImage = 0;

	// Setup the description of the texture.
	textureDesc.Height = height;
	textureDesc.Width = width;
	textureDesc.MipLevels = 0;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
	// Create the empty texture.
	if (FAILED(device->CreateTexture2D(&textureDesc, NULL, &m_2Dtexture))) {
		return false;
	}

	// Set the row pitch of the targa image data.
	rowPitch = (width * 4) * sizeof(unsigned char);

	// Copy the targa image data into the texture.
	deviceContext->UpdateSubresource(m_2Dtexture, 0, NULL, targaData, rowPitch, 0);

	// Setup the shader resource view description.
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;

	// Create the shader resource view for the texture.
	if (FAILED(device->CreateShaderResourceView(m_2Dtexture, &srvDesc, &m_textureView))) {
		return false;
	}

	// Generate mipmaps for this texture.
	deviceContext->GenerateMips(m_textureView);

	// Release the targa image data now that the image data has been loaded into the texture.
	delete[] targaData;
	targaData = 0;

	return true;

}
