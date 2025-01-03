#pragma once
#include "sgg/graphics.h"
#include "gameobject.h"
#include "box.h"


class Shield : public GameObject, public Box {
	graphics::Brush m_brush_shield;

	static Shield* m_unique_instance;

	float offset = 0.6f;
	float m_width = 0.15f;
	float m_height = 0.7f;

	bool m_isLeftPlayer = false;
	bool m_left_turn_processed = false;
	bool m_right_turn_processed = false;
	int m_turn_counter = 0;

public:
	Shield();
	~Shield() {}

	void init() override;
	void draw() override;
	void update(float dt) override;
	void reset();

	float& getPosX() { return m_pos_x; }
	float& getPosY() { return m_pos_y; }
	float& getWidth() { return m_width; }
	float& getHeight() { return m_height; }

	static Shield* getInstance();

	Box toBox() const { return Box(m_pos_x, m_pos_y, m_width, m_height); }

protected:
	void debugDraw();
};
