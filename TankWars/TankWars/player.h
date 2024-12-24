#pragma once
#include "gameobject.h"
#include "box.h"
#include "sgg/graphics.h"

class Player : public GameObject, public Box
{
	graphics::Brush m_brush_player;

	bool isLeftPlayer = NULL;
	float m_cannon_degrees = 0.0f;

public:
	Player(std::string name, bool isLeft) : GameObject(name), isLeftPlayer(isLeft) {}

	void init() override;
	void draw() override;
	void update(float dt) override;

	void setCannonDegrees(float degrees);
	float getCannonDegrees();
};
