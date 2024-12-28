#pragma once
#include "sgg/graphics.h"
#include <string>

class GameState
{
private:
	std::string m_asset_path = "assets\\";

	float m_canvas_width = 12.0f;
	float m_canvas_height = 6.0f;

	static GameState* m_unique_instance;

	GameState();

	class Player* m_playerLeft = 0;
	class Player* m_playerRight = 0;
	class Level* m_current_level = 0;
	bool isLeftTurn = true;

public:
	float m_global_offset_x = 0.0f;
	float m_global_offset_y = 0.0f;
	bool m_debugging = false;

	void init();
	void draw();
	void update(float dt);
	void updateIsLeftTurn();
	bool getIsLeftTurn();

	static GameState* getInstance();
	~GameState();

	float getCanvasWidth() { return m_canvas_width; }
	float getCanvasHeight() { return m_canvas_height; }

	std::string getAssetDir();
	std::string getFullAssetPath(const std::string & asset);

	class Player* getPlayerLeft() { return m_playerLeft; }
	class Player* getPlayerRight() { return m_playerRight; }
};
