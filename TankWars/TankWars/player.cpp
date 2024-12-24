#include "player.h"
#include "gamestate.h"
#include "util.h"
#include <iostream>

void Player::init()
{
	if (isLeftPlayer) {
		m_pos_x = 2.0f;
	}
	else {
		m_pos_x = 6.0f;
	}
	m_pos_y = 3.0f;

	m_width = 0.78f;  // x is 2.62 times the y
	m_height = 0.3f;

	m_state->m_global_offset_x = m_state->getCanvasWidth() / 2.0f - m_pos_x;
	m_state->m_global_offset_y = m_state->getCanvasHeight() / 2.0f - m_pos_y;

	SETCOLOR(m_brush_player.fill_color, 1.0f, 1.0f, 1.0f);
	m_brush_player.fill_opacity = 1.0f;
	m_brush_player.outline_opacity = 0.0f;

	if (isLeftPlayer) {
		m_brush_player.texture = m_state->getInstance()->getFullAssetPath("tank_left.png");
	}
	else {
		m_brush_player.texture = m_state->getInstance()->getFullAssetPath("tank_right.png");
	}
}

void Player::draw()
{
	graphics::drawRect(m_pos_x, m_pos_y, m_width, m_height, m_brush_player);
}

void Player::update(float dt)
{
	// float p = 1.0f + fabs(cos(graphics::getGlobalTime() / 2000.0f)) * 0.8f;
	// SETCOLOR(global_state.m_brush_background.fill_color, 2.4f * p, 0.06f * p, 0.06f * p);

	float delta_time = dt / 1000.0f;
	const float velocity = 0.8f;

	if (graphics::getKeyState(graphics::SCANCODE_A))
		m_pos_x -= velocity * delta_time;
	if (graphics::getKeyState(graphics::SCANCODE_D))
		m_pos_x += velocity * delta_time;
	if (graphics::getKeyState(graphics::SCANCODE_W) and m_cannon_degrees < 90.0f)
		m_cannon_degrees += velocity * delta_time;
	if (graphics::getKeyState(graphics::SCANCODE_S) and m_cannon_degrees > 0.0f)
		m_cannon_degrees -= velocity * delta_time;

	m_state->m_global_offset_x = m_state->getCanvasWidth() / 2.0f - m_pos_x;
	m_state->m_global_offset_y = m_state->getCanvasHeight() / 2.0f - m_pos_y;

	GameObject::update(dt);
}

void Player::setCannonDegrees(float degrees)
{
	m_cannon_degrees = degrees;
}

float Player::getCannonDegrees()
{
	return m_cannon_degrees;
}

float m_cannon_degrees = 0;
