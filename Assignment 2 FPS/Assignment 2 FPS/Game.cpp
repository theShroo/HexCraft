/*	FIT2096 - Example Code
*	Game.cpp
*	Created by Elliott Wilson & Mike Yeates - 2016 - Monash University
*	overhauled by Russell Chidambaranathan 2017 - FIT2096
*/

#include "Game.h"
#include "Bullets.h"
#include "AIPlayer.h"
#include "Cell.h"

using namespace DirectX;

Game::Game()
{
	m_renderer = 0;
	m_input = 0;
	m_player = 0;
	m_score = 0;
	m_debug = false;
	// seed the random number generator
	m_generator = std::default_random_engine(unsigned(std::chrono::system_clock::now().time_since_epoch().count()));
	m_inventoryIndex = 0;
	// flags for dynamic render distance are set to default values.
	m_activeDistance = 3;
	m_clustersize = 8;
	m_updated = false;
	for (int i = 0; i < 4; i++) {
		m_debugstrings.push_back(new std::wstring);
	}
}

Game::~Game() {}

// initialise the Game Object
bool Game::Initialise(Direct3D* renderer, InputController* input)
{
	m_renderer = renderer;	
	m_input = input;
	InitShaders();
	LoadMeshes();
	LoadTextures();
	InitGameWorld();
	LoadUI();
	RefreshUI();
	return true;
}

bool Game::InitShaders()
{
	// new shaderloader and mini manager use cleaner code.
	Shader::LoadShader("Unlit Texture Shader", L"Assets/Shaders/VertexShader.vs", L"Assets/Shaders/UnlitTexturedPixelShader.ps", m_renderer->GetDevice());
	Shader::LoadShader("Diffuse Texture Fog Shader", L"Assets/Shaders/VertexShader.vs", L"Assets/Shaders/DiffuseTexturedFogPixelShader.ps", m_renderer->GetDevice());
	return true;
}

bool Game::LoadMeshes()
{
	// new mesh loader and mini manager use cleaner code.
	Mesh::LoadMesh("Hexagon", "Assets/Meshes/hexagonal_brick.obj", m_renderer);
	Mesh::LoadMesh("Enemy", "Assets/Meshes/enemy.obj", m_renderer);
	Mesh::LoadMesh("Bullet", "Assets/Meshes/bullet.obj", m_renderer);
	Mesh::LoadMesh("Ammo", "Assets/Meshes/ammoBlock.obj", m_renderer);
	Mesh::LoadMesh("Ruby", "Assets/Meshes/ruby.obj", m_renderer);
	return true;
}

bool Game::LoadTextures()
{
	// new texture loader and minimanager use cleaner code. also loads targa images.
	Texture::LoadTexture(m_renderer, "Dirt", "Assets/Textures/brick_dirt.png");
	Texture::LoadTexture(m_renderer, "Cobblestone", "Assets/Textures/brick_cobblestone.png");
	Texture::LoadTexture(m_renderer, "Player Light Red", "Assets/Textures/gradient_redLighter.png");
	Texture::LoadTexture(m_renderer, "Button", "Assets/Textures/button.png");
	Texture::LoadTexture(m_renderer, "Bullet", "Assets/Textures/bullet.png");
	Texture::LoadTexture(m_renderer, "Wall", "Assets/Textures/tile_disabled.png");
	Texture::LoadTexture(m_renderer, "Ammo", "Assets/Textures/item_box.png");
	Texture::LoadTexture(m_renderer, "Cross Hair", "Assets/Textures/cross_hair.tga");
	Texture::LoadTexture(m_renderer, "Health", "Assets/Textures/sprite_healthBar.png");
	Texture::LoadTexture(m_renderer, "Hurt", "Assets/Textures/sprite_hurtOverlay.png");
	Texture::LoadTexture(m_renderer, "Air", "Null");
	Texture::LoadTexture(m_renderer, "Inventory", "Assets/Textures/inventory.png");
	Texture::LoadTexture(m_renderer, "Inventory Item", "Assets/Textures/inventoryItem.png");

	return true;
}

void Game::LoadUI()
{
	// the spritemanager requires a static initialisation.
	SpriteManager::Initialise(m_renderer);

	SpriteManager::LoadFont("Assets/Fonts/Arial-12pt.spritefont", "Ariel 12");
	SpriteManager::LoadFont("Assets/Fonts/Arial-18pt.spritefont", "Ariel 18");
	SpriteManager::LoadFont("Assets/Fonts/Arial-23pt.spritefont", "Ariel 23");

	// initialise the statemachine
	m_stateMachine = new StateMachine<GameState, Game>(this, MainMenu);

	// an extra spritemanager for debugging.
	m_debugOverlay = new SpriteManager();

	m_debugOverlay->MakeSpriteFont("Ariel 18", m_debugstrings[0], XMFLOAT2(20, 200), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 0, XMFLOAT2(0, 0));
	m_debugOverlay->MakeSpriteFont("Ariel 18", m_debugstrings[1], XMFLOAT2(20, 230), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 0, XMFLOAT2(0, 0));
	m_debugOverlay->MakeSpriteFont("Ariel 18", m_debugstrings[2], XMFLOAT2(20, 260), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 0, XMFLOAT2(0, 0));
	m_debugOverlay->MakeSpriteFont("Ariel 18", m_debugstrings[3], XMFLOAT2(20, 290), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 0, XMFLOAT2(0, 0));

	// initialise the spriteManagers.
	std::pair<int, int> size = m_input->GetWindowSize();
	// main menu
	m_stateMachine->RegisterState(MainMenu, 0, 0, 0, 0);
	m_stateMachine->Spritemanager(MainMenu)->MakeButton(m_input, XMINT2(128, 64), Texture::GetTexture("Button"), "Click to Begin!", XMFLOAT2(size.first / 2, 200), "Ariel 12", [this] {m_stateMachine->ChangeState(Active); });
	m_stateMachine->Spritemanager(MainMenu)->MakeSpriteFont("Ariel 23", L"Hexcraft!", XMFLOAT2((size.first - 200) / 2, 100), XMFLOAT4(0.0f, 0.5f, 1.0f, 1.0f), 0, XMFLOAT2(1, 0));
	// game active state
	m_stateMachine->RegisterState(Active, 0, ActiveUpdateFunction(),
		// game render lambda
		[this]() { m_map->RenderLocal(m_player->GetPosition(), m_renderer, m_player->GetCamera());
			if (m_debug) {
				m_debugOverlay->Render();
		}
	}, 0);

	m_stateMachine->Spritemanager(Active)->MakeSprite(Texture::GetTexture("Cross Hair"), XMFLOAT2((size.first - 20.0f) / 2.0f, (size.second - 20.0f) / 2.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(20,20));
	m_stateMachine->Spritemanager(Active)->MakeSprite(Texture::GetTexture("Health"), XMFLOAT2(25, size.second - 50), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), XMFLOAT2(m_player->GetHealth(), 40));
	m_stateMachine->Spritemanager(Active)->MakeSprite(Texture::GetTexture("Hurt"), XMFLOAT2(0,0), XMFLOAT4(1.0f, 0.0f, 0.0f, 0.0f), XMFLOAT2(size.first,size.second));
	m_stateMachine->Spritemanager(Active)->MakeSpriteFont("Ariel 18", &m_highScoreText, XMFLOAT2(20, 150), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 0, XMFLOAT2(0, 0));
	m_stateMachine->Spritemanager(Active)->MakeSpriteFont("Ariel 18", &m_player->GetAmmoCounter(), XMFLOAT2(20, 180), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 0, XMFLOAT2(0, 0));
	// win
	m_stateMachine->RegisterState(Victory, 0, 0, 0, 0);
	m_stateMachine->Spritemanager(Victory)->MakeSpriteFont("Ariel 18", L"You Win!", XMFLOAT2((size.first - 20.0f) /2.0f, 180.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 0, XMFLOAT2(0, 0));
	// loose
	m_stateMachine->RegisterState(GameOver, 0, 0, 0, 0);
	m_stateMachine->Spritemanager(GameOver)->MakeSpriteFont("Ariel 18", L"You Loose!", XMFLOAT2((size.first - 20.0f) / 2.0f, 180.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 0, XMFLOAT2(0, 0));
	// loose
	m_stateMachine->RegisterState(GameOver, 0, 0, 0, 0);
	m_stateMachine->Spritemanager(GameOver)->MakeSpriteFont("Ariel 18", L"You Loose!", XMFLOAT2((size.first - 20.0f) / 2.0f, 180.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 0, XMFLOAT2(0, 0));
	// pause
	m_stateMachine->RegisterState(Paused, 0, 
		// pause menu update lambda
		[this](float timestep) {			
			if (m_input->GetKeyDown(VK_ESCAPE)) {
			m_stateMachine->ChangeState(Active);
			}
		}, 0, 0);
	m_stateMachine->Spritemanager(Paused)->MakeButton(m_input, XMINT2(128, 64), Texture::GetTexture("Button"), "Resume Game", XMFLOAT2(size.first / 2, 200), "Ariel 12", [this] {m_stateMachine->ChangeState(Active); });
	m_stateMachine->Spritemanager(Paused)->MakeButton(m_input, XMINT2(128, 64), Texture::GetTexture("Button"), "Inventory", XMFLOAT2(size.first / 2, 300), "Ariel 12", [this] {m_stateMachine->ChangeState(Inventory); });
	m_stateMachine->Spritemanager(Paused)->MakeSpriteFont("Ariel 23", L"Paused", XMFLOAT2((size.first - 125) / 2, 100), XMFLOAT4(0.0f, 0.5f, 1.0f, 1.0f), 0, XMFLOAT2(0, 0));

	// Inventory
	m_stateMachine->RegisterState(Inventory, 
		// enter inventory screen lambda
		[this]() {
		m_inventoryIndex = 0;
			for (unsigned i = 0; i < 12; i++) {
				m_stateMachine->Spritemanager(Inventory)->GetButton(i)->SetButtonText(MathsHelper::ConvertString(m_player->GetEquipmentDescription(i).c_str()));
			}
		},
		// inventory update lambda
		[this](float timestep) {
			if (m_input->GetKeyDown(VK_ESCAPE)) {
				m_stateMachine->ChangeState(Active);
			}
			if (m_input->GetKeyDown(VK_UP)) {
				if (m_inventoryIndex < 3) {
					m_inventoryIndex = 0;
				}
			}
			if (m_input->GetKeyDown(VK_DOWN)) {
				if (m_inventoryIndex < m_player->GetInventory()->size() - 39) {
					m_inventoryIndex += 3;
				}
			}
			std::unordered_map <PointerKey, std::pair<Equipment*, int>, PointerHash>::iterator iter;
			iter = m_player->GetInventory()->begin();
			advance(iter, m_inventoryIndex);
			for (int i = 12; i < 51; i++) {
				if (iter != m_player->GetInventory()->end()) {
					std::string desc = iter->second.first->GetName().c_str();
					if (iter->second.second > 1) {
						desc += " (" + std::to_string(iter->second.second) + ")";
					}
					m_stateMachine->Spritemanager(Inventory)->GetButton(i)->SetButtonText(MathsHelper::ConvertString(desc.c_str()));
					iter++;
				}
				else {
					m_stateMachine->Spritemanager(Inventory)->GetButton(i)->SetButtonText(L"Empty");
				}
			}
		}, 0, 0);
	// backdrop for inventory screen
	m_stateMachine->Spritemanager(Inventory)->MakeSprite(Texture::GetTexture("Inventory"), XMFLOAT2((size.first - 1024.0f) / 2.0f, (size.second - 768.0f) / 2.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 0.8f), XMFLOAT2(1024, 768));
	m_stateMachine->Spritemanager(Inventory)->MakeSpriteFont("Ariel 23", L"Inventory", XMFLOAT2((size.first - 200.0f) / 2.0f, 100.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 0, XMFLOAT2(0, 0));
	// make all the buttons for the equipped items side of the inventory screen.
	for (int i = 0; i < 12; i++) {
		m_stateMachine->Spritemanager(Inventory)->MakeButton(m_input, XMINT2(200, 50), Texture::GetTexture("Inventory Item"), "Main Hand", XMFLOAT2((size.first - 1024.0f) / 2.0f + 136, (size.second - 768.0f) / 2.0f + 99 + (56 * i)), "Ariel 12", [this](int index) { m_player->Unequip(index+1); m_stateMachine->Spritemanager(Inventory)->GetButton(index)->SetButtonText(MathsHelper::ConvertString(m_player->GetEquipmentDescription(index).c_str())); }, i);
	}
	// make all the buttons for the inventory side of the screen
	for (int i = 0; i < 13; i++) {
		for (int j = 0; j < 3; j++) {
			m_stateMachine->Spritemanager(Inventory)->MakeButton(m_input, XMINT2(200, 50), Texture::GetTexture("Inventory Item"), "Empty", XMFLOAT2((size.first - 1024.0f) / 2.0f + 487 + (207 * j), (size.second - 768.0f) / 2.0f + 48 + (56 * i)), "Ariel 12",
				// on click lambda for inventory buttons, includes callback data to allow these buttons to call player equip options.
				[this](int index) {
					if (m_player->GetInventory()->size() > index) {
						std::unordered_map <PointerKey, std::pair<Equipment*, int>, PointerHash>::iterator iter;
						iter = m_player->GetInventory()->begin();
						advance(iter, index);
						int slot = iter->second.first->m_slot_required - 1;
						if (slot >= 0) {
							m_player->Equip(iter->second.first->GetName());
							m_stateMachine->Spritemanager(Inventory)->GetButton(slot)->SetButtonText(MathsHelper::ConvertString(m_player->GetEquipmentDescription(slot).c_str()));
						}
					}
			}, i * 3 + j);			
		}
	}
}


void Game::RefreshUI()
{
	// Ensure text in UI matches latest scores etc (call this after data changes)
	// Concatenate data into our label string using a wide string stream
	m_score = float(m_activeDistance);
	std::wstringstream ss;
	ss << L"Render distance: " << m_score;
	m_highScoreText = ss.str();

	*m_debugstrings[0] = m_player->GetPositionString();
	Hex location = Hex::VectorToHex(m_player->GetPosition());
	*m_debugstrings[1] = L"Hex location: X: " + std::to_wstring(location.x) + L", Y: " + std::to_wstring(location.y);
	location = Hex::smalltobig(m_currentPosition, m_clustersize);
	*m_debugstrings[2] = L"physical Cluster location: X: " + std::to_wstring(location.x) + L", Y: " + std::to_wstring(location.y);
	location = Hex::bigtosmall(m_player->GetLocation()->GetCluster()->GetLocation(), m_clustersize);
	*m_debugstrings[3] = L"big to small location: X: " + std::to_wstring(location.x) + L", Y: " + std::to_wstring(location.y);


	if (m_input->GetKeyDown(VK_F3)) {
		if (m_debug) {
			m_debug = false;
			m_map->AddObject(m_player);
		}
		else {
			m_debug = true;
			m_player->GetLocation()->GetEntities()->erase(m_player->operator PointerKey());
		}
	}

}

void Game::InitGameWorld()
{
	// create the map
	m_map = new Map(m_clustersize);

	// bullets and equipment need initialising before all their functionality performs properly.
	Bullets::Initialise(m_map);
	Equipment::Initialise();
	// add the player
	m_player = new FPSPlayer(MathsHelper::GetXMVECTOR3(0, 0, 0), "Diffuse Texture Fog Shader", "Enemy", "Player Light Red", m_input);
	// add the player to the map
	m_map->AddObject(m_player);
	m_currentPosition = Hex{ 0,0,0,0 };

	m_map->Initialise(m_currentPosition, m_activeDistance);

	m_map->Update(0, m_player->GetPosition());
}
void Game::Update(float timestep)
{
	m_input->BeginUpdate();
	m_stateMachine->Update(timestep);

	RefreshUI();
	m_input->EndUpdate();
	
}

void Game::Render()
{
	m_renderer->BeginScene(0.6f, 0.6f, 0.9f, 1.0f);
	m_stateMachine->Render();
	m_renderer->EndScene();
}



void Game::Shutdown()
{
	if( m_map){
		delete m_map;
		m_map = 0;
	}

	// new managers have easy release functions
	Shader::Release();
	Mesh::Release();
	Texture::Release();
	SpriteManager::Release();
}

std::function<void(float a)> Game::ActiveUpdateFunction() {
	return [this](float timestep) {
		int hp = m_player->GetHealth();


		// in debug mode the player is updated independently of the map.
		if (m_debug) {
			m_player->Update(timestep);
		}
		else 
		{
			// quick hak to test/prevent the players cluster from falling out of the update cycle.
			Cluster* current = m_player->GetLocation()->GetCluster();
			m_map->GetActiveClusters()->operator[](*current) = current;
			// TODO work out why there is a problem transfering the player from one cluster to another and maintaining the update process.
			// for some reason when the player (and presumably any other entitiy) moves from one cluster to another the new sector is dropped from the update list.
			// this is a critical problem.
		}
		// dynamic zone incrementation to increment the zone as long as fps exceeds 60fps.
		if (m_updated && timestep < 0.00833) {
			
			if (!m_map->IncrementZone(m_currentPosition, m_activeDistance)) {
				m_activeDistance++;
			}
			
		}
		// dynamic zone decrementation if the performance falls below 30fps
		if (timestep > 0.1666) {
			if (m_activeDistance > 2) {
				m_activeDistance--;
			}
		}
		// zone update to update renderables and updateables as the player moves around
		if (m_player->GetLocation()->GetLocation() != m_currentPosition) {
			Hex oldcluster = m_map->GetCell(m_currentPosition)->GetCluster()->GetLocation();
			Hex newcluster = m_player->GetLocation()->GetCluster()->GetLocation();
			if (oldcluster != newcluster) {
				m_map->UpdateZones(newcluster, newcluster - oldcluster, m_activeDistance);
			}
			m_currentPosition = m_player->GetLocation()->GetLocation();
			m_updated = true;
		}
		else {
			m_updated = false;
		}
		// update overlays
		RECT& health = m_stateMachine->Spritemanager(Active)->GetSprite(1)->Rectangle;
		XMVECTOR& hurt = m_stateMachine->Spritemanager(Active)->GetSprite(2)->Colour;
		XMVECTOR speedA = m_player->GetVelocity();
		// access menus
		if (m_input->GetKeyDown(VK_ESCAPE)) {
			m_stateMachine->ChangeState(Paused);
		}
		// perform the actual map update
		m_map->Update(timestep, m_player->GetPosition());
		// check players health for the hurt overlay
		hp -= m_player->GetHealth();
		health.right = health.left + m_player->GetHealth();
		hurt *= MathsHelper::GetXMVECTOR4(1, 1, 1, 0.95f);
		if (hp) {
			hurt = MathsHelper::GetXMVECTOR4(1, 1, 1, 1);
		}
		m_debugOverlay->Update(timestep);

		if (m_player->GetHealth() <= 0) {
			m_stateMachine->ChangeState(GameOver);
		}
		std::pair<int, int> size = m_input->GetWindowSize();
		RECT window;
		GetWindowRect(m_input->GetWindow(), &window);
		SetCursorPos((window.left + window.right) / 2, (window.top + window.bottom) / 2);

	};

}