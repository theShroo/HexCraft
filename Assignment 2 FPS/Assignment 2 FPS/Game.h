/*	FIT2096 - Example Code
*	Game.h
*	Created by Elliott Wilson & Mike Yeates - 2016 - Monash University
*	This class is the heart of our game and is also where our game logic will reside
*	It contains the overall Update and Render method for the whole game
*	You should eventually split some game logic into other objects which Game will manage
*	Watch the size of this class - it can grow out of control very fast!
*
*	this class has been overhauled completely by Russell Chidambaranathan 2017, 
*	very little original code remains but the general layout was retained
*/

#pragma once

#ifndef GAME_H
#define GAME_H



#include "MathsHelper.h"
#include "Map.h"
#include "Input.h"
#include "FPSPlayer.h"
#include "StateMachine.h"

// enumerate game states
enum GameState {
	MainMenu = 1,
	Active,
	Inventory,
	Paused,
	GameOver,
	Victory
};

// enumerate debug states
// we are being sneaky with out states, and this is actually an array of bits, 
// bit 1 = debug overlay
// bit 2 = movement disassociation

enum DebugState{
	NoDebug = 0,
	OverlayOnly,
	MovementOnly,
	Both
};


class Game
{
private:
	StateMachine<GameState, Game>* m_stateMachine;
	Direct3D* m_renderer;
	InputController* m_input;

	// Gameobjects
	FPSPlayer* m_player;
	Map* m_map;

	// inventory management and display variables, to store current index of the inventory, and to store descriptions for renderering the names of the equipped items.
	unsigned m_inventoryIndex;


	// Initialisation Helpers
	bool InitShaders();
	bool LoadMeshes();
	bool LoadTextures();
	void LoadUI();
	void InitGameWorld();

	void RefreshUI();
	// debug stuff
	DebugState m_debug;
	SpriteManager* m_debugOverlay;
	std::vector<std::wstring*> m_debugstrings;
	// performance optimisation data for performance update 1.
	HexLogic::Hex m_currentPosition;
	std::function<void(float a)> Game::ActiveUpdateFunction();
	unsigned m_activeDistance;
	bool m_updated;
	int m_clustersize;

public:
	Game();	
	~Game();
	std::default_random_engine m_generator;

	bool Initialise(Direct3D* renderer, InputController* input); //The initialise method will load all of the content for the game (meshes, textures, etc.)

	void Update(float timestep);	//The overall Update method for the game. All gameplay logic will be done somewhere within this method
	void Render();					//The overall Render method for the game. Here all of the meshes that need to be drawn will be drawn
	void Shutdown();				//Cleanup everything we initialised
};

#endif