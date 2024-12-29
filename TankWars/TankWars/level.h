#pragma once
#include "sgg/graphics.h"
#include "gameobject.h"
#include "box.h"
#include <list>
#include <vector>
#include <string>


class Level : public GameObject
{
	graphics::Brush m_brush_background;

	std::vector<GameObject*> m_static_objects;
	std::list<GameObject*> m_dynamic_objects;

	float m_center_x = 5.0f;
	float m_center_y = 5.0f;

	const float m_shield_size_x = 0.2f;
	const float m_shield_size_y = 0.8f;

	const float m_block_size_x = 1.0f;
	const float m_block_size_y = 0.2f;
	std::vector<Box> m_blocks;
	std::vector<std::string> m_block_names;
	graphics::Brush m_block_brush;
	graphics::Brush m_block_brush_debug;

	void drawBlock(int i);
	void checkCollisions();

public:
	Level(const std::string& name = "Level0");
	~Level();

	void init() override;
	void draw() override;
	void update(float dt) override;
};
