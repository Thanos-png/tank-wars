#pragma once
#include "sgg/graphics.h"
#include "gameobject.h"
#include "box.h"


class Shot : public GameObject, public Box {
	graphics::Brush m_brush_shot;

	static Shot* m_unique_instance;

	float radius = 0.1f;
	float m_cannon_degrees = 0.0f;

	// The minimum that we want the gravity to be is 2 and the maximum is 30, but we want the user to choose anywhere from 0 to 100
	// So we are gona use this function to convert the value to the ones we want.  gravity_function(x) = -0.28 * x + 30
	const float m_gravity = 5.0f;
	const float m_accel_vertical = 6.0f;
	const float m_accel_horizontal = 13.0f;  // 21.0f
	const float m_max_velocity = 3.0f;

public:
	Shot();
	~Shot() {}

	void init() override;
	void draw() override;
	void update(float dt) override;
	void reset();

	float m_vx = 0.0f;
	float m_vy = 0.0f;

	float& getPosX() { return m_pos_x; }
	float& getPosY() { return m_pos_y; }

	void setCannonDegrees(float cannonDegrees);
	float getCannonDegrees();
	void setRadius(float newRadius);
	float getRadius();

	static Shot* getInstance();
	bool intersect(Box& other);

protected:
	void debugDraw();
	void shotBullet(float dt);
};
