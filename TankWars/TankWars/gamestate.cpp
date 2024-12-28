#include "gamestate.h"
#include "level.h"
#include "player.h"

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

	if (!m_current_level)
		return;

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
