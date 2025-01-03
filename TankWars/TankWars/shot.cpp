#include "shot.h"
#include "gamestate.h"
#include "player.h"
#include "util.h"


Shot::Shot()
{
	GameObject::setActive(false);
}

void Shot::init()
{
	GameObject::setActive(true);

	if (m_state->getPlayerLeft()->isActive() and m_state->getIsLeftTurn()) {
		m_pos_x = m_state->getPlayerLeft()->m_pos_x;
		m_pos_y = m_state->getPlayerLeft()->m_pos_y;
	}
	if (m_state->getPlayerRight()->isActive() and not m_state->getIsLeftTurn()) {
		m_pos_x = m_state->getPlayerRight()->m_pos_x;
		m_pos_y = m_state->getPlayerRight()->m_pos_y;
	}

	m_width = 0.2f;
	m_height = 0.2f;

	// m_state->m_global_offset_x = m_state->getCanvasWidth() / 2.0f - m_pos_x;
	// m_state->m_global_offset_y = m_state->getCanvasHeight() / 2.0f - m_pos_y;

	SETCOLOR(m_brush_shot.fill_color, 1.0f, 1.0f, 1.0f);
	m_brush_shot.fill_opacity = 1.0f;
	m_brush_shot.outline_opacity = 0.0f;
	m_brush_shot.texture = m_state->getInstance()->getFullAssetPath("cannonball.png");
}

void Shot::draw()
{
	float x = m_pos_x + m_state->m_global_offset_x;
	float y = m_pos_y + m_state->m_global_offset_y;

	graphics::drawRect(x, y, m_width, m_height, m_brush_shot);

	if (m_state->m_debugging)
		debugDraw();
}

void Shot::update(float dt)
{
	shotBullet(dt);  // -------------------------------------------------------------------------------

	// m_state->m_global_offset_x = m_state->getCanvasWidth() / 2.0f - m_pos_x;
	// m_state->m_global_offset_y = m_state->getCanvasHeight() / 2.0f - m_pos_y;

	// In case the shot has bugged and gone below the surface
	if (m_pos_y >= 20.0f) {
		m_vx = 0;
		m_vy = 0;
		reset();
		return;
	}

	GameObject::update(dt);
}

void Shot::reset()
{
	// Reset the shot position
	m_pos_x = 0.0f;
	m_pos_y = 0.0f;

	// Reset velocities
	m_vx = 0.0f;
	m_vy = 0.0f;

	if (m_state->getPlayerLeft()->isActive() and m_state->getIsLeftTurn())
		m_state->getPlayerLeft()->setShootingFlag(false);
	if (m_state->getPlayerRight()->isActive() and not m_state->getIsLeftTurn())
		m_state->getPlayerRight()->setShootingFlag(false);

	// Deactivate the shield
	setActive(false);
}

void Shot::setCannonDegrees(float cannonDegrees)
{
	m_cannon_degrees = cannonDegrees;
}

float Shot::getCannonDegrees()
{
	return m_cannon_degrees;
}

void Shot::setRadius(float newRadius)
{
	radius = newRadius;
}

float Shot::getRadius()
{
	return radius;
}

Shot* Shot::getInstance()
{
	if (m_unique_instance == nullptr)
		m_unique_instance = new Shot();

	return m_unique_instance;
}

void Shot::debugDraw()
{
	float x = m_pos_x + m_state->m_global_offset_x;
	float y = m_pos_y + m_state->m_global_offset_y;

	graphics::Brush debug_brush;
	SETCOLOR(debug_brush.fill_color, 1, 0.3f, 0);
	SETCOLOR(debug_brush.outline_color, 1, 0.1f, 0);
	debug_brush.fill_opacity = 0.1f;
	debug_brush.outline_opacity = 1.0f;
	graphics::drawRect(x, y, m_width, m_height, debug_brush);

	char s[20];
	sprintf_s(s, "(%5.2f, %5.2f)", x, y);
	SETCOLOR(debug_brush.fill_color, 1, 0, 0);
	debug_brush.fill_opacity = 1.0f;
	graphics::drawText(x - 0.5f * m_width, y - 0.7f * m_height, 0.15f, s, debug_brush);
}

void Shot::shotBullet(float dt)
{
	float delta_time = dt / 1000.0f;
	float move = 0.0f;

	if (m_state->getPlayerLeft()->isActive() and m_state->getIsLeftTurn()) {
		move += 1.0f;
	}
	if (m_state->getPlayerRight()->isActive() and not m_state->getIsLeftTurn()) {
		move -= 1.0f;
	}

	// X
	m_vx = std::min(m_max_velocity, m_vx + delta_time * move * m_accel_horizontal);
	m_vx = std::max(-m_max_velocity, m_vx);
	m_vx -= 0.2f * m_vx / (0.1f + fabs(m_vx));  // * 1.01f;
	if (fabs(m_vx) < 0.01f) { m_vx = 0.0f; }
	m_pos_x += m_vx * delta_time / 0.2f;

	// Y
	if (m_vy == 0.0f) {
		m_vy -= m_accel_vertical;
	}
	m_vy += delta_time * m_gravity;
	m_pos_y += m_vy * delta_time;
}

// Circle-Box Collision Detection
bool Shot::intersect(Box& other) {
	// Adjust shot position with offsets
	float adjusted_x = m_pos_x + m_state->m_global_offset_x;
	float adjusted_y = m_pos_y + m_state->m_global_offset_y;

	// Calculate the closest point on the box to the circle
	float closest_x = clamp(adjusted_x, other.m_pos_x - other.m_width / 2.0f, other.m_pos_x + other.m_width / 2.0f);
	float closest_y = clamp(adjusted_y, other.m_pos_y - other.m_height / 2.0f, other.m_pos_y + other.m_height / 2.0f);

	// Calculate the distance between the circle's center and this closest point
	float distance_x = adjusted_x - closest_x;
	float distance_y = adjusted_y - closest_y;

	return (distance_x * distance_x + distance_y * distance_y) < (radius * radius);
}

Shot* Shot::m_unique_instance = nullptr;
