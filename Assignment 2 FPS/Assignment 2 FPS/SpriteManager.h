#ifndef SPRITEMANAGER_H
#define SPRITEMANAGER_H


#include "DirectXTK/SpriteBatch.h"
#include "DirectXTK/SpriteFont.h"
#include "DirectXTK/CommonStates.h"
#include "MathsHelper.h"
#include "Direct3D.h"
#include "Texture.h"
#include "Button.h"


struct SpriteFontPointerData {
	DirectX::XMVECTOR Location, Colour, Efects;
	std::wstring* Text;
	float rotation;
	std::string Font;
};
struct SpriteFontData {
	DirectX::XMVECTOR Location, Colour, Efects;
	std::wstring Text;
	float rotation;
	std::string Font;
};

struct SpritePointerData {
	Texture** texture;
	RECT Rectangle;
	DirectX::XMVECTOR Colour;
};
struct SpriteData {
	Texture* texture;
	RECT Rectangle;
	DirectX::XMVECTOR Colour;
};

class SpriteManager{
public:
	static void Initialise(Direct3D* renderer);
	static void Release();
	static DirectX::SpriteFont* GetFont(std::string FontID);
	static void LoadFont(char* filePath, std::string fontID);
	SpriteManager();
	~SpriteManager();
	void Render();
	DirectX::SpriteBatch* GetBatch() { return m_spriteBatch; }

	// makespritefont takes a pointer to a text object, this allows changes to the text to be reflected in the sprite wihout updating the sprite.
	void MakeSpriteFont(std::string font, std::wstring* text, DirectX::XMFLOAT2 location, DirectX::XMFLOAT4 colour, float rotation, DirectX::XMFLOAT2 effects);
	void MakeSpriteFont(std::string font, std::wstring text, DirectX::XMFLOAT2 location, DirectX::XMFLOAT4 colour, float rotation, DirectX::XMFLOAT2 effects);
	// makebutton takes all the usual arguments to create a button but creates it as a subobject in the manager, this allows us to 
	// only render and activate buttons in the active spritemanager, and to allow us to create multiple screens.

	void MakeButton(InputController* input, DirectX::XMINT2 size, Texture* texture, std::string buttontext, DirectX::XMFLOAT2 location, std::string font, std::function<void()> onClickFunction);
	void MakeButton(InputController* input, DirectX::XMINT2 size, Texture* texture, std::string buttontext, DirectX::XMFLOAT2 location, std::string font, std::function<void(int a)> onClickFunction, int index);
	Button* GetButton(unsigned index) { if (index < m_buttons->size()) { return m_buttons->operator[](index); } else { return 0; } }
	// makesprite takes a pointer to a pointer to a texture. this means that the pointer passed to the sprite may be changed to point to a different texture,  
	// and that the new texture will be rendered by the sprite. 
	unsigned MakeSprite(Texture** texture, DirectX::XMFLOAT2 location, DirectX::XMFLOAT4 colour, DirectX::XMFLOAT2 size);
	unsigned MakeSprite(Texture* texture, DirectX::XMFLOAT2 location, DirectX::XMFLOAT4 colour, DirectX::XMFLOAT2 size);
	SpriteData* GetSprite(unsigned index) { return m_sprites->operator[](index); }
	void Update(float timestep);
private:
	static std::unordered_map<std::string, SpriteManager*> m_managers;
	static std::unordered_map<std::string, DirectX::SpriteFont*> m_fonts;
	static Direct3D* m_renderer;
	DirectX::SpriteBatch* m_spriteBatch;
	std::vector<SpriteFontData*>* m_fontSprites;
	std::vector<SpriteFontPointerData*>*m_fontSpritePointers;
	std::vector<SpritePointerData*>* m_spritePointers;
	std::vector<SpriteData*>* m_sprites;
	std::vector<Button*>* m_buttons;
};



#endif