#include "gamestate.h"
#include "level.h"
#include "player.h"
#include <thread>
#include <chrono>


GameState::GameState()
{
}

GameState::~GameState()
{
	if (m_playerLeft)
		delete m_playerLeft;
	if (m_playerRight)
		delete m_playerRight;
	if (m_current_level)
		delete m_current_level;
}

void GameState::init()
{
	m_current_level = new Level("Level1");
	m_current_level->init();

	m_playerLeft = new Player("Player1", true);
	m_playerLeft->init();
	m_playerRight = new Player("Player2", false);
	m_playerRight->init();

	graphics::preloadBitmaps(getAssetDir());
	graphics::setFont(m_asset_path + "Broken-Detroit.ttf");
}

void GameState::draw()
{
	if (!m_current_level)
		return;

	m_current_level->draw();
}

void GameState::update(float dt)
{
	// Skip updating if a long delay is detected to avoid messing up the collision simulation.
	if (dt > 500)  // ms
		return;

	float sleep_time = std::max(0.0f, 17.0f - dt);

	std::this_thread::sleep_for(std::chrono::duration<float, std::milli>());

	if (!m_current_level)
		return;

	// Update offsets dynamically
	// m_global_offset_x += 0.01f * dt; // Adjust based on game logic
	// m_global_offset_y += 0.01f * dt; // Adjust based on game logic

	m_current_level->update(dt);

	m_debugging = graphics::getKeyState(graphics::SCANCODE_0);
}

void GameState::updateIsLeftTurn()
{
	if (isLeftTurn) {
		isLeftTurn = false;
	}
	else {
		isLeftTurn = true;
	}
}

bool GameState::getIsLeftTurn()
{
	return isLeftTurn;
}

GameState* GameState::getInstance()
{
	if (m_unique_instance == nullptr)
		m_unique_instance = new GameState();

	return m_unique_instance;
}

std::string GameState::getAssetDir()
{
	return m_asset_path;
}

std::string GameState::getFullAssetPath(const std::string& asset)
{
	return m_asset_path + asset;
}

GameState* GameState::m_unique_instance = nullptr;
