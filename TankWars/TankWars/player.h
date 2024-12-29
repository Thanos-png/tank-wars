#pragma once
#include "sgg/graphics.h"
#include "gameobject.h"
#include "box.h"
#include "shot.h"

	
class Player : public GameObject, public Box
{
	class Shot* m_shot = 0;

	graphics::Brush m_brush_player;
	std::vector<std::string> m_sprites;

	bool m_shootingFlag = false;
	bool m_isLeftPlayer = NULL;
	float m_cannon_degrees = 0.0f;

public:
	Player(std::string name, bool isLeft);
	~Player();

	void init() override;
	void draw() override;
	void update(float dt) override;

	void setCannonDegrees(float degrees);
	float getCannonDegrees();

	void setShootingFlag(bool flag);
	bool getShootingFlag();

	Shot* getShotInstance();

protected:
	void debugDraw();
};
