#include "gamestate.h"
#include "level.h"
#include "player.h"

GameState::GameState()
{
}

GameState::~GameState()
{
	if (m_player1)
		delete m_player1;
	if (m_player2)
		delete m_player2;
	if (m_current_level)
		delete m_current_level;
}

void GameState::init()
{
	m_current_level = new Level("Level1");
	m_current_level->init();

	m_player1 = new Player("Player1", true);
	m_player1->init();
	m_player2 = new Player("Player2", false);
	m_player2->init();

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
	if (dt > 500)
		return;

	if (!m_current_level)
		return;

	m_current_level->update(dt);
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
