/*	Texture class updated by Russell Chidambaranathan 2017.
*
*	texture manager and texture class, 
*	this class provides simple accessor and loader functions to simplify the process of managing
*	textures.
*	some code from Monash university, modified and expanded upon for fit2096
*/



#ifndef TEXTURE_H
#define TEXTURE_H


#include <d3d11.h>
#include <stdio.h>
#include <iostream>
#include <unordered_map>
#include <string>

#include "Direct3D.h"



class Texture {
	struct TargaHeader {
		unsigned char data1[12];
		unsigned short width;
		unsigned short height;
		unsigned char bpp;
		unsigned char data2;
	};
private:
	bool Initialise(Direct3D* renderer, char* filename);
	bool LoadTarga(char*, Direct3D* renderer);
	ID3D11Resource* m_texture;
	ID3D11Texture2D* m_2Dtexture;

	ID3D11ShaderResourceView* m_textureView;
	Texture();
	Texture(Direct3D* renderer, char* filename);
	~Texture();
	void Terminate();
public:
	static std::unordered_map<std::string, Texture*> textures;
	static void LoadTexture(Direct3D* renderer, std::string textureID,  char* filename);
	static Texture* GetTexture(std::string textureID);
	static void Release();

	ID3D11ShaderResourceView* GetShaderResourceView() { return m_textureView; }

};
#endif // !TEXTURE_H
