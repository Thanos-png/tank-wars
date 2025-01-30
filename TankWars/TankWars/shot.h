#pragma once
#include "sgg/graphics.h"
#include "gameobject.h"
#include "box.h"


class Shot : public GameObject, public Box {
	graphics::Brush m_brush_shot;

	static Shot* m_unique_instance;

	float radius = 0.1f;

	const float m_gravity = 5.0f;
	const float m_accel_vertical = 9.0f;
	const float m_accel_horizontal = 11.0f;
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

	void setRadius(float newRadius);
	float getRadius();

	static Shot* getInstance();
	bool intersect(Box& other);

protected:
	void debugDraw();
	void shotBullet(float dt);
};
