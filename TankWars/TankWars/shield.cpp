#include "shield.h"
#include "gamestate.h"
#include "player.h"
#include "level.h"
#include "util.h"


Shield::Shield()
{
	GameObject::setActive(false);
}

void Shield::init()
{
	GameObject::setActive(true);
	isShield = true;

	if (m_state->getPlayerLeft()->isActive() and m_state->getIsLeftTurn()) {
		m_pos_x = m_state->getPlayerLeft()->getPosX() + offset;
		m_pos_y = m_state->getCurrentLevel()->getGroundLevel(m_pos_x) - 0.32f;

		m_isLeftPlayer = true;
	}
	if (m_state->getPlayerRight()->isActive() and not m_state->getIsLeftTurn()) {
		m_pos_x = m_state->getPlayerRight()->getPosX() - offset;
		m_pos_y = m_state->getCurrentLevel()->getGroundLevel(m_pos_x) - 0.32f;

		m_isLeftPlayer = false;
	}

	SETCOLOR(m_brush_shield.fill_color, 1.0f, 1.0f, 1.0f);
	m_brush_shield.fill_opacity = 1.0f;
	m_brush_shield.outline_opacity = 0.0f;
	m_brush_shield.texture = m_state->getInstance()->getFullAssetPath("shield.png");
}

void Shield::draw()
{
	float x = getPosX() + m_state->m_global_offset_x;
	float y = getPosY() + m_state->m_global_offset_y;

	graphics::drawRect(x, y, m_width, m_height, m_brush_shield);

	if (m_state->m_debugging)
		debugDraw();
}

void Shield::update(float dt)
{
	// Check if it's the left player's turn
	if (m_state->getPlayerLeft()->isActive() and m_state->getIsLeftTurn() and not m_left_turn_processed) {
		m_turn_counter += 1;  // Increment the total turn counter
		m_left_turn_processed = true;  // Mark this turn as processed
		m_right_turn_processed = false;  // Reset the turn processed flag
	}

	// Check if it's the right player's turn
	if (m_state->getPlayerRight()->isActive() and not m_state->getIsLeftTurn() and not m_right_turn_processed) {
		m_turn_counter += 1;  // Increment the total turn counter
		m_right_turn_processed = true;  // Mark this turn as processed
		m_left_turn_processed = false;  // Reset the turn processed flag
	}

	if (m_turn_counter >= 5) {
		reset();
		return;
	}

	GameObject::update(dt);
}

void Shield::reset()
{
	// Reset the deployed_shield value
	if (m_state->getPlayerLeft()->isActive() and m_state->getIsLeftTurn())
		m_state->getPlayerLeft()->setDeployedShield(false);
	if (m_state->getPlayerRight()->isActive() and not m_state->getIsLeftTurn())
		m_state->getPlayerRight()->setDeployedShield(false);

	// Decrease shield count by one
	if (m_state->getPlayerLeft()->isActive() and m_state->getIsLeftTurn())
		m_state->getPlayerLeft()->decreaseShieldAmount();
	if (m_state->getPlayerRight()->isActive() and not m_state->getIsLeftTurn())
		m_state->getPlayerRight()->decreaseShieldAmount();

	// Deactivate the shield
	setActive(false);
}

Shield* Shield::getInstance()
{
	if (m_unique_instance == nullptr)
		m_unique_instance = new Shield();

	return m_unique_instance;
}

void Shield::debugDraw()
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
	graphics::drawText(x - 1.7f * m_width, y - 0.7f * m_height, 0.15f, s, debug_brush);
}

Shield* Shield::m_unique_instance = nullptr;
