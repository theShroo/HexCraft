/*	FIT2096 - Example Code
*	Button.h
*	Created by Elliott Wilson - 2015 - Monash University
*	This class represents a Button in our scene
*	It uses the sprite batch and a sprite font to render an image and the string on screen
*	It also supports a hover state image which is achieved via a texture that contains the
*	normal button texture on top and the hover state texture on the bottom (see Button.png for example)
*	aditions to the code by Russell Chidambaranathan.
*/

#ifndef BUTTON_H
#define BUTTON_H

#include <functional>

#include "Direct3D.h"
#include "DirectXTK/SpriteBatch.h"
#include "DirectXTK/SpriteFont.h"

#include "Texture.h"
#include "Input.h"


class Button
{
private:
	int m_width;				//This is the width of the button, it should be set to the same width as the texture we're using
	int m_height;				//This the actual height of the button on screen, so it will be half of the height of the texture
	Texture* m_buttonTexture;	//This is the texture for the button. The sprite batch will be drawing a portion of this texture
	LPCWSTR m_text;				//This is the string that will appear on the button. It's a Wide (Unicode) string because the sprite font uses them

	DirectX::XMFLOAT2 m_position;			//This is the screen position of the button, it is measured from the centre of the button texture
	DirectX::XMFLOAT2 m_origin;			//This vector will be the origin of the button, we will set it to the centre of the texture
	DirectX::XMFLOAT2 m_textOrigin;		//This the origin of the text, we'll look at how we set this in the constructor
	RECT m_textureWindow;		//This RECT define which portion of the texture we are going to draw
	DirectX::XMFLOAT4 m_textColour;			//This colour will define the colour of the text on the button
	int m_index;
	DirectX::SpriteBatch* m_spriteBatch;	//We need the sprite batch to draw the texture
	DirectX::SpriteFont* m_font;			//We need the font to draw the text
	InputController* m_input;	//We need the input controller to find out what the mouse is up to

	bool m_isEnabled;

	//This is a function object, they are new in C++11 and allow us to do some pretty amazing things. This function object is going to store a "lambda expression" for us.
	//This lambda expression is going to contain the code that will run when this button is clicked. See Game.cpp for how we actually define the lambda expression
	std::function<void()> m_onClickFunction;
	std::function<void(int a)> m_onClickFunction_1;
public:
	//The constructor takes in all of the required information including the function object that we want to call when the button is clicked
	Button(int width, int height, Texture* texture, LPCWSTR text, DirectX::XMFLOAT2 position,
		DirectX::SpriteBatch* spriteBatch, DirectX::SpriteFont* font, InputController* input, std::function<void()> onClickFunction);
	// new constructor to allow for the implementation of unique button data to be stored in the button at creation and passed to the button lambda on click.
	Button(int width, int height, Texture* texture, LPCWSTR text, DirectX::XMFLOAT2 position,
		DirectX::SpriteBatch* spriteBatch, DirectX::SpriteFont* font, InputController* input, std::function<void(int a)> onClickFunction, int index);
	~Button();

	//Mutators
	void SetTextColour(DirectX::XMFLOAT4 colour) { m_textColour = colour; }
	void SetButtonText(LPCWSTR text) { m_text = text; }

	void Enable() { m_isEnabled = true; }
	void Disable() { m_isEnabled = false; }

	void Update();		//The Update method is going to be in charge of dealing with the mouse input
	void Render();		//The Render method is going to make all of the calls to the sprite batch to render the texture and text
};

#endif