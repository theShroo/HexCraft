/*	SpriteManager.cpp written by Russell Chidambaranathan 2017.
*	
*	a purpose built sprite manager, to handle any number of spritefonts that may be desired.
*	this class provides simple accessor and loader functions to simplify the process of managing
*	sprites, fonts and buttons. 
*/


#include "SpriteManager.h"
using namespace DirectX;

std::unordered_map<std::string, SpriteManager*> SpriteManager::m_managers;
std::unordered_map<std::string, SpriteFont*> SpriteManager::m_fonts;
Direct3D* SpriteManager::m_renderer = 0;

// the sprite manager needs a renderer before any other functions can be called
void SpriteManager::Initialise(Direct3D* renderer) {
	m_renderer = renderer;
}


void SpriteManager::LoadFont(char* filePath, std::string fontID) {
	if (m_fonts.count(fontID) > 0) {
		delete m_fonts[fontID];
		m_fonts[fontID] = 0;
	}
	wchar_t* path = MathsHelper::ConvertString(filePath);
	if (m_renderer) {
		m_fonts[fontID] = new SpriteFont(m_renderer->GetDevice(), path);
	}
	else {
		RaiseException(6, 0, 0, 0);
	}
}

SpriteFont* SpriteManager::GetFont(std::string FontID) {
	if (m_fonts.count(FontID) > 0) {
		return m_fonts[FontID];
	}
	else {
		RaiseException(5, 0, 0, 0);
		return NULL;
	}
}


void SpriteManager::Release() {
	{
		// upon release destroy all managers
		std::unordered_map<std::string, SpriteManager*>::iterator terminator;
		for (terminator = m_managers.begin(); terminator != m_managers.end(); terminator++) {
			if (terminator->second) {
				delete terminator->second;
				terminator->second = 0;
			}
		}
		m_managers.clear();
	}

	// and all loaded fonts.
	{
		std::unordered_map<std::string, SpriteFont*>::iterator terminator;
		for (terminator = m_fonts.begin(); terminator != m_fonts.end(); terminator++) {
			if (terminator->second) {
				delete terminator->second;
				terminator->second = 0;
			}
		}
		m_fonts.clear();
	}
	m_renderer = 0;
}


SpriteManager::SpriteManager() {
	if (m_renderer) {
		m_spriteBatch = new SpriteBatch(m_renderer->GetDeviceContext());
		m_fontSprites = new std::vector<SpriteFontData*>;
		m_sprites = new std::vector<SpriteData*>;
		m_buttons = new std::vector<Button*>;
		m_fontSpritePointers = new std::vector<SpriteFontPointerData*>;
		m_spritePointers = new std::vector<SpritePointerData*>;
	}
	else {
		RaiseException(5,0,0,0);
	}
}
// upon deletion of an individual manager that manager must destroy all its assets.
SpriteManager::~SpriteManager() {
	if (m_fontSprites) {
		for (int i = 0; i < m_fontSprites->size(); i++) {
			delete m_fontSprites->operator[](i);
		}
		m_fontSprites->clear();
		delete m_fontSprites;
		m_fontSprites = 0;
	}
	if (m_fontSpritePointers) {
		for (int i = 0; i < m_fontSpritePointers->size(); i++) {
			delete m_fontSpritePointers->operator[](i);
		}
		m_fontSpritePointers->clear();
		delete m_fontSpritePointers;
		m_fontSpritePointers = 0;
	}

	if (m_spriteBatch) {
		delete m_spriteBatch;
		m_spriteBatch = 0;
	}

	if (m_spritePointers) {
		for (int i = 0; i < m_spritePointers->size(); i++) {
			delete m_spritePointers->operator[](i);
		}
		m_spritePointers->clear();
		delete m_spritePointers;
		m_spritePointers = 0;
	}

	if (m_sprites) {
		for (int i = 0; i < m_sprites->size(); i++) {
			delete m_sprites->operator[](i);
		}
		m_sprites->clear();
		delete m_sprites;
		m_sprites = 0;
	}
	if (m_buttons) {
		for (int i = 0; i < m_buttons->size(); i++) {
			delete m_buttons->operator[](i);
		}
		m_buttons->clear();
		delete m_buttons;
		m_buttons = 0;
	}
}


unsigned SpriteManager::MakeSprite(Texture** texture, XMFLOAT2 location, XMFLOAT4 colour, XMFLOAT2 size) {
	SpritePointerData* sprite = new SpritePointerData;
	sprite->texture = texture;
	sprite->Rectangle = RECT{long(location.x),long(location.y),long(location.x+size.x),long(location.y + size.y)};
	sprite->Colour = MathsHelper::GetXMVECTOR4(colour);
	m_spritePointers->push_back(sprite);
	return m_sprites->size() -1;

}

unsigned SpriteManager::MakeSprite(Texture* texture, XMFLOAT2 location, XMFLOAT4 colour, XMFLOAT2 size) {
	SpriteData* sprite = new SpriteData;
	sprite->texture = texture;
	sprite->Rectangle = RECT{ long(location.x),long(location.y),long(location.x + size.x),long(location.y + size.y) };
	sprite->Colour = MathsHelper::GetXMVECTOR4(colour);
	m_sprites->push_back(sprite);
	return m_sprites->size() - 1;

}


void SpriteManager::MakeSpriteFont(std::string font, std::wstring text, XMFLOAT2 location, XMFLOAT4 colour, float rotation, XMFLOAT2 effects) {
	SpriteFontData* sprite = new SpriteFontData;
	sprite->Font = font;
	sprite->Location = MathsHelper::GetXMVECTOR2(location);
	sprite->Colour = MathsHelper::GetXMVECTOR4(colour);
	sprite->Efects = MathsHelper::GetXMVECTOR2(effects);
	sprite->rotation = rotation;
	sprite->Text = text;
	m_fontSprites->push_back(sprite);
}

void SpriteManager::MakeSpriteFont(std::string font, std::wstring* text, XMFLOAT2 location, XMFLOAT4 colour, float rotation, XMFLOAT2 effects) {
	SpriteFontPointerData* sprite = new SpriteFontPointerData;
	sprite->Font = font;
	sprite->Location = MathsHelper::GetXMVECTOR2(location);
	sprite->Colour = MathsHelper::GetXMVECTOR4(colour);
	sprite->Efects = MathsHelper::GetXMVECTOR2(effects);
	sprite->rotation = rotation;
	sprite->Text = text;
	m_fontSpritePointers->push_back(sprite);
}

void SpriteManager::MakeButton(InputController* input, XMINT2 size, Texture* texture, std::string buttontext, XMFLOAT2 location, std::string font, std::function<void()> onClickFunction) {
	Button* button = new Button(size.x, size.y, texture, MathsHelper::ConvertString(buttontext.c_str()), location, m_spriteBatch, GetFont(font), input, onClickFunction);
	m_buttons->push_back(button);
}
// new button constructor to allow for iteratively created button lambdas with unique callback data.
void SpriteManager::MakeButton(InputController* input, XMINT2 size, Texture* texture, std::string buttontext, XMFLOAT2 location, std::string font, std::function<void(int a)> onClickFunction, int index) {
	Button* button = new Button(size.x, size.y, texture, MathsHelper::ConvertString(buttontext.c_str()), location, m_spriteBatch, GetFont(font), input, onClickFunction, index);
	m_buttons->push_back(button);
}


void SpriteManager::Render() {
	// initialise the sprite renderer;
	m_renderer->SetCurrentShader(NULL);
	CommonStates states(m_renderer->GetDevice());
	m_spriteBatch->Begin(SpriteSortMode_Deferred, states.NonPremultiplied());

	// render all sprites handled by this manager.
	for (int i = 0; i < m_sprites->size(); i++) {
		SpriteData* data = m_sprites->operator[](i);
		m_spriteBatch->Draw((data->texture)->GetShaderResourceView(), data->Rectangle, data->Colour);
	}
	
	// render all sprite pointers handled by this manager.
	for (int i = 0; i < m_spritePointers->size(); i++) {
		SpritePointerData* data = m_spritePointers->operator[](i);
		m_spriteBatch->Draw((*data->texture)->GetShaderResourceView(), data->Rectangle, data->Colour);
	}
	// and text	
	for (int i = 0; i < m_fontSprites->size(); i++) {
		SpriteFontData* data = m_fontSprites->operator[](i);
		GetFont(data->Font)->DrawString(m_spriteBatch, data->Text.c_str(), data->Location, data->Colour, data->rotation, data->Efects);
	}	
	// and text pointers
	for (int i = 0; i < m_fontSpritePointers->size(); i++) {
		SpriteFontPointerData* data = m_fontSpritePointers->operator[](i);
		GetFont(data->Font)->DrawString(m_spriteBatch, data->Text->c_str(), data->Location, data->Colour, data->rotation, data->Efects);
	}
	// and buttons
	for (int i = 0; i < m_buttons->size(); i++) {
		m_buttons->operator[](i)->Render();
	}

	m_spriteBatch->End();


}

void SpriteManager::Update(float timestep) {
	for (int i = 0; i < m_buttons->size(); i++) {
		m_buttons->operator[](i)->Update();
	}
}