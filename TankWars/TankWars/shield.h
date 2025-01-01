#pragma once
#include "sgg/graphics.h"
#include "gameobject.h"
#include "box.h"


class Shield : public GameObject, public Box {
	graphics::Brush m_brush_shield;

	static Shield* m_unique_instance;

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

	static Shield* getInstance();

protected:
	void debugDraw();
};
