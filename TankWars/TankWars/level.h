#pragma once
#include "sgg/graphics.h"
#include "gameobject.h"
#include "box.h"
#include <list>
#include <vector>
#include <string>
#include <functional>


struct GroundFunction {
	float start_x; // Start of the range
	float end_x;   // End of the range
	std::function<float(float)> func; // The ground function for this range
};

class Level : public GameObject
{
	graphics::Brush m_brush_background;
	graphics::Brush m_brush_foreground;
	graphics::Brush m_block_brush;
	graphics::Brush m_block_brush_debug;
	graphics::Brush m_score_brush;

	std::vector<GameObject*> m_static_objects;
	std::list<GameObject*> m_dynamic_objects;

	int m_score_left = 0;
	int m_score_right = 0;
	int m_left_base_health = 2;
	int m_right_base_health = 2;

	float m_center_x = 5.0f;
	float m_center_y = 5.0f;

	const float m_block_size_x = 1.5f;
	const float m_block_size_y = 0.2f;

	std::vector<Box> m_blocks;
	std::vector<std::string> m_block_names;

	std::vector<GroundFunction> m_ground_functions; // Multiple ground functions representing the ground-level

	void drawBlock(int i);
	void checkCollisions();

public:
	Level(const std::string& name = "Level0");
	~Level();

	void init() override;
	void draw() override;
	void update(float dt) override;

	std::vector<Box>& getBlocks();

	void setGroundFunction(float start_x, float end_x, std::function<float(float)> func);
	float getGroundLevel(float x);
	float getGroundGradient(float x);
};
