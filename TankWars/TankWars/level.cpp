#include "level.h"
#include "gamestate.h"
#include "player.h"
#include "util.h"
#include <cmath>


Level::Level(const std::string& name)
{
}

Level::~Level()
{
	for (auto p_gob : m_static_objects)
		if (p_gob) delete p_gob;

	for (auto p_gob : m_dynamic_objects)
		if (p_gob) delete p_gob;
}

const float e = 2.71828f;

void Level::init()
{
	setGroundFunction(0.0f, 2.6f, [](float x) -> float { return sin(x - 1.2f) + 4; });
	setGroundFunction(2.6f, 10.4f, [](float x) -> float { return (pow(sin((x - 6.5f) / 2.5f), 2) + 29) / 6; });
	setGroundFunction(10.4f, 14.0f, [](float x) -> float { return 1.1f * pow(e, -pow(((x / 2) - 6.8f), 4)) + 5; });
	setGroundFunction(14.0f, 19.0f, [](float x) -> float { return 2.75f * pow(e, (-pow((x - 14), 2)) / 5) + 3.35f; });
	setGroundFunction(19.0f, 21.1f, [](float x) -> float { return -(pow((x - 19), 3) / 7) + 3.36f; });
	setGroundFunction(21.1f, 21.7f, [](float x) -> float { return 0.8f * pow(x - 21.9f, 2) + 1.45f; });
	setGroundFunction(21.7f, 23.87f, [](float x) -> float { return 1.25f * pow(x - 22, 2) + 1.4f; });
	setGroundFunction(23.87f, 27.0f, [](float x) -> float { return 2 * pow(e, -pow((x - 26.5f), 4) / 100) + 4.5f; });
	setGroundFunction(27.0f, 32.0f, [](float x) -> float { return 1.1f * pow(e, -pow(x - 28.7f, 2)) + 6.45f; });
	setGroundFunction(32.0f, 34.0f, [](float x) -> float { return (pow(x - 32, 2) / 4 ) + 6.45f; });

	SETCOLOR(m_brush_background.fill_color, 1.0f, 1.0f, 1.0f);
	m_brush_background.fill_opacity = 1.0f;
	m_brush_background.outline_opacity = 0.0f;
	m_brush_background.texture = m_state->getInstance()->getFullAssetPath("background.png");

	SETCOLOR(m_brush_foreground.fill_color, 1.0f, 1.0f, 1.0f);
	m_brush_foreground.fill_opacity = 1.0f;
	m_brush_foreground.outline_opacity = 0.0f;
	m_brush_foreground.texture = m_state->getInstance()->getFullAssetPath("ground-function.png");

	for (auto p_gob : m_static_objects)
		if (p_gob) p_gob->init();

	for (auto p_gob : m_dynamic_objects)
		if (p_gob) p_gob->init();

	Box leftBase = Box(6, 0.45f, m_block_size_x, m_block_size_y);
	leftBase.isLeftBase = true;
	m_blocks.push_back(leftBase);

	Box rightBase = Box(31.5f, 0.45f, m_block_size_x, m_block_size_y);
	rightBase.isRightBase = true;
	m_blocks.push_back(rightBase);
	
	m_block_names.push_back("left-base.png");
	m_block_names.push_back("right-base.png");

	m_block_brush.outline_opacity = 0.0f;
	m_block_brush_debug.fill_opacity = 0.1f;
	SETCOLOR(m_block_brush_debug.fill_color, 0.2f, 1.0f, 0.1f);
	SETCOLOR(m_block_brush_debug.outline_color, 0.3f, 1.0f, 0.2f);
}

void Level::draw()
{
	float w = 2.82f * m_state->getCanvasWidth();
	float h = 2.86f * m_state->getCanvasHeight();

	float background_offset_x = m_state->m_global_offset_x / 5.0f + w / 2.5f;
	float background_offset_y = m_state->m_global_offset_y / 5.0f + h / 3.5f;
	float foreground_offset_x = m_state->m_global_offset_x + w / 2.0f;
	float foreground_offset_y = m_state->m_global_offset_y + h / 2.0f;

	// Draw background
	graphics::drawRect(background_offset_x, background_offset_y, w, h, m_brush_background);

	// Draw foreground
	graphics::drawRect(foreground_offset_x, foreground_offset_y, w, h, m_brush_foreground);

	// Draw Players
	if (m_state->getPlayerLeft()->isActive())
		m_state->getPlayerLeft()->draw();
	if (m_state->getPlayerRight()->isActive())
		m_state->getPlayerRight()->draw();

	// Draw Blocks
	for (int i = 0; i < m_blocks.size(); i++)
		drawBlock(i);

	// Draw score
	char score[20];
	sprintf_s(score, "Score: %d", m_score_left);
	graphics::drawText(0.15f, 0.3f, 0.3f, score, m_score_brush);

	sprintf_s(score, "Score: %d", m_score_right);
	graphics::drawText(10.6f, 0.3f, 0.3f, score, m_score_brush);

	for (auto p_gob : m_static_objects)
		if (p_gob) p_gob->draw();

	for (auto p_gob : m_dynamic_objects)
		if (p_gob) p_gob->draw();

	// graphics::Brush m_brush_testing;
	// graphics::drawRect(2, 2.45f, 1, 0.2f, m_brush_testing);
}

void Level::update(float dt)
{
	if (m_state->getPlayerLeft()->isActive() and m_state->getIsLeftTurn())
		m_state->getPlayerLeft()->update(dt);
	if (m_state->getPlayerRight()->isActive() and not m_state->getIsLeftTurn())
		m_state->getPlayerRight()->update(dt);

	checkCollisions();

	GameObject::update(dt);
}

std::vector<Box>& Level::getBlocks()
{
	return m_blocks;
}

void Level::setGroundFunction(float start_x, float end_x, std::function<float(float)> func)
{
	m_ground_functions.push_back({ start_x, end_x, func });
}

float Level::getGroundLevel(float x)
{
	for (const auto& gf : m_ground_functions) {
		if (x >= gf.start_x && x <= gf.end_x) {
			return gf.func(x);  // Evaluate the corresponding ground function
		}
	}
	return 0.0f;  // Default ground level if no function is defined for the range
}

void Level::drawBlock(int i)
{
	Box& box = m_blocks[i];
	float x = box.m_pos_x + m_state->m_global_offset_x;
	float y = -box.m_pos_y + getGroundLevel(box.m_pos_x) + m_state->m_global_offset_y;
	m_block_brush.texture = m_state->getFullAssetPath(m_block_names[i]);

	graphics::drawRect(x, y, m_block_size_x, m_block_size_y, m_block_brush);

	if (m_state->m_debugging)
		graphics::drawRect(x, y, m_block_size_x, m_block_size_y, m_block_brush_debug);
}

void Level::checkCollisions()
{
	// Check if the shot collides with a Box
	for (auto& box : m_blocks) {
		Box adjustedBox = box;
		adjustedBox.m_pos_x += m_state->m_global_offset_x;
		adjustedBox.m_pos_y += m_state->m_global_offset_y;

		if (m_state->getIsLeftTurn() and m_state->getPlayerLeft()->getShotInstance()->isActive() and
			m_state->getPlayerLeft()->getShotInstance()->intersect(adjustedBox) and m_state->getPlayerLeft()->getShootingFlag()) {
			// Collision detected, deactivate shot
			m_state->getPlayerLeft()->getShotInstance()->reset();

			graphics::playSound(m_state->getFullAssetPath("ground-collision.wav"), 0.5f);

			// Destroy Base
			if (adjustedBox.isRightBase) {
				m_right_base_health -= 1;
				if (m_right_base_health <= 0) {
					// Find and remove the right base from m_blocks
					m_blocks.erase(std::remove_if(m_blocks.begin(), m_blocks.end(),
						[](const Box& box) { return box.isRightBase; }),
						m_blocks.end());

					// Find and Remove the corresponding texture from m_block_names
					m_block_names.erase(std::remove_if(m_block_names.begin(), m_block_names.end(),
						[](const std::string& name) { return name == "right-base.png"; }),
						m_block_names.end());
				}
			}
			break;
		}

		if (not m_state->getIsLeftTurn() and m_state->getPlayerRight()->getShotInstance()->isActive() and
			m_state->getPlayerRight()->getShotInstance()->intersect(adjustedBox) and m_state->getPlayerRight()->getShootingFlag()) {
			// Collision detected, deactivate shot
			m_state->getPlayerRight()->getShotInstance()->reset();

			graphics::playSound(m_state->getFullAssetPath("ground-collision.wav"), 0.5f);

			// Destroy Base
			if (adjustedBox.isLeftBase) {
				m_left_base_health -= 1;
				if (m_left_base_health <= 0) {
					// Find and remove the right base from m_blocks
					m_blocks.erase(std::remove_if(m_blocks.begin(), m_blocks.end(),
						[](const Box& box) { return box.isLeftBase; }),
						m_blocks.end());

					// Find and Remove the corresponding texture from m_block_names
					m_block_names.erase(std::remove_if(m_block_names.begin(), m_block_names.end(),
						[](const std::string& name) { return name == "left-base.png"; }),
						m_block_names.end());
				}
			}
			break;
		}
	}

	// Check if the shot or the player collides with a shield
	float offset;
	if ((m_state->getPlayerRight()->getShieldInstance()->isActive() and m_state->getPlayerRight()->getDeployedShield()) or
		(m_state->getPlayerLeft()->getShieldInstance()->isActive() and m_state->getPlayerLeft()->getDeployedShield())) {
		float left_shield_x = m_state->getPlayerLeft()->getShieldInstance()->getPosX() + m_state->m_global_offset_x;
		float left_shield_y = m_state->getPlayerLeft()->getShieldInstance()->getPosY() + m_state->m_global_offset_y;
		float left_shield_width = m_state->getPlayerLeft()->getShieldInstance()->getWidth();
		float left_shield_height = m_state->getPlayerLeft()->getShieldInstance()->getHeight();
		Box leftShield = Box(left_shield_x, left_shield_y, left_shield_width, left_shield_height);

		float right_shield_x = m_state->getPlayerRight()->getShieldInstance()->getPosX() + m_state->m_global_offset_x;
		float right_shield_y = m_state->getPlayerRight()->getShieldInstance()->getPosY() + m_state->m_global_offset_y;
		float right_shield_width = m_state->getPlayerRight()->getShieldInstance()->getWidth();
		float right_shield_height = m_state->getPlayerRight()->getShieldInstance()->getHeight();
		Box rightShield = Box(right_shield_x, right_shield_y, right_shield_width, right_shield_height);

		// Destroy right shield (left shot collides with right shield)
		if (m_state->getPlayerLeft()->getShotInstance()->isActive() and m_state->getPlayerLeft()->getShotInstance()->intersect(rightShield) and
			m_state->getPlayerLeft()->getShootingFlag() and m_state->getPlayerRight()->getDeployedShield()) {

			m_state->getPlayerLeft()->getShotInstance()->reset();
			m_state->getPlayerRight()->getShieldInstance()->reset();
			
			graphics::playSound(m_state->getFullAssetPath("ground-collision.wav"), 0.5f);
			return;
		}

		// Destroy left shield (right shot collides with left shield)
		if (m_state->getPlayerRight()->getShotInstance()->isActive() and m_state->getPlayerRight()->getShotInstance()->intersect(leftShield) and
			m_state->getPlayerRight()->getShootingFlag() and m_state->getPlayerLeft()->getDeployedShield()) {

			m_state->getPlayerRight()->getShotInstance()->reset();
			m_state->getPlayerLeft()->getShieldInstance()->reset();
			
			graphics::playSound(m_state->getFullAssetPath("ground-collision.wav"), 0.5f);
			return;
		}

		// Collision detection (left shot collides with left shield)
		if (m_state->getPlayerLeft()->getShotInstance()->isActive() and m_state->getPlayerLeft()->getShotInstance()->intersect(leftShield) and
			m_state->getPlayerLeft()->getShootingFlag() and m_state->getPlayerLeft()->getDeployedShield()) {

			m_state->getPlayerLeft()->getShotInstance()->reset();
			
			graphics::playSound(m_state->getFullAssetPath("ground-collision.wav"), 0.5f);
			return;
		}

		// Collision detection (right shot collides with right shield)
		if (m_state->getPlayerRight()->getShotInstance()->isActive() and m_state->getPlayerRight()->getShotInstance()->intersect(rightShield) and
			m_state->getPlayerRight()->getShootingFlag() and m_state->getPlayerRight()->getDeployedShield()) {

			m_state->getPlayerRight()->getShotInstance()->reset();
			
			graphics::playSound(m_state->getFullAssetPath("ground-collision.wav"), 0.5f);
			return;
		}

		// Check if the player collides with a shield
		offset = 0.0f;
		if (m_state->getPlayerLeft()->intersectSideways(leftShield) and m_state->getPlayerLeft()->getDeployedShield()) {
			offset = m_state->getPlayerLeft()->intersectSideways(leftShield);
			m_state->getPlayerLeft()->m_pos_x += offset;
		}
		if (m_state->getPlayerLeft()->intersectSideways(rightShield) and m_state->getPlayerRight()->getDeployedShield()) {
			offset = m_state->getPlayerLeft()->intersectSideways(rightShield);
			m_state->getPlayerLeft()->m_pos_x += offset;
		}
		if (m_state->getPlayerRight()->intersectSideways(leftShield) and m_state->getPlayerLeft()->getDeployedShield()) {
			offset = m_state->getPlayerRight()->intersectSideways(leftShield);
			m_state->getPlayerRight()->m_pos_x += offset;
		}
		if (m_state->getPlayerRight()->intersectSideways(rightShield) and m_state->getPlayerRight()->getDeployedShield()) {
			offset = m_state->getPlayerRight()->intersectSideways(rightShield);
			m_state->getPlayerRight()->m_pos_x += offset;
		}
	}

	// Check if a player collides with a Box
	/*
	for (auto& box : m_blocks) {
		offset = 0.0f;

		// Check for the left player's collision
		if (offset = m_state->getPlayerLeft()->intersectSideways(box) and m_state->getIsLeftTurn()) {
			m_state->getPlayerLeft()->m_pos_x += offset;
			break;
		}

		// Check for the right player's collision
		if (offset = m_state->getPlayerRight()->intersectSideways(box) and not m_state->getIsLeftTurn()) {
			m_state->getPlayerRight()->m_pos_x += offset;
			break;
		}
	}
	*/

	// Check if the player's shot collides with the enemy player
	float right_player_x = m_state->getPlayerRight()->getPosX() + m_state->m_global_offset_x;
	float right_player_y = m_state->getPlayerRight()->getPosY() + m_state->m_global_offset_y;
	float right_player_width = m_state->getPlayerRight()->getWidth();
	float right_player_height = m_state->getPlayerRight()->getHeight();
	Box player_right = Box(right_player_x, right_player_y, right_player_width, right_player_height);
	if (m_state->getIsLeftTurn() and m_state->getPlayerLeft()->getShotInstance()->isActive() and
		m_state->getPlayerLeft()->getShotInstance()->intersect(player_right) and m_state->getPlayerLeft()->getShootingFlag()) {

		m_state->getPlayerLeft()->getShotInstance()->reset();
		m_score_left += 1;

		graphics::playSound(m_state->getFullAssetPath("ground-collision.wav"), 0.5f);
		return;
	}

	float left_player_x = m_state->getPlayerLeft()->getPosX() + m_state->m_global_offset_x;
	float left_player_y = m_state->getPlayerLeft()->getPosY() + m_state->m_global_offset_y;
	float left_player_width = m_state->getPlayerLeft()->getWidth();
	float left_player_height = m_state->getPlayerLeft()->getHeight();
	Box player_left = Box(left_player_x, left_player_y, left_player_width, left_player_height);
	if (not m_state->getIsLeftTurn() and m_state->getPlayerRight()->getShotInstance()->isActive() and
		m_state->getPlayerRight()->getShotInstance()->intersect(player_left) and m_state->getPlayerRight()->getShootingFlag()) {

		m_state->getPlayerRight()->getShotInstance()->reset();
		m_score_right += 1;

		graphics::playSound(m_state->getFullAssetPath("ground-collision.wav"), 0.5f);
		return;
	}

	// Check for ground collision
	if (m_state->getPlayerLeft()->getShotInstance()->isActive() and m_state->getPlayerLeft()->getShootingFlag()) {
		float shot_x = m_state->getPlayerLeft()->getShotInstance()->getPosX();
		float shot_y = m_state->getPlayerLeft()->getShotInstance()->getPosY();
		if (shot_y >= getGroundLevel(shot_x)) {
			m_state->getPlayerLeft()->getShotInstance()->reset();

			graphics::playSound(m_state->getFullAssetPath("ground-collision.wav"), 0.5f);
			return;
		}
	}

	if (m_state->getPlayerRight()->getShotInstance()->isActive() and m_state->getPlayerRight()->getShootingFlag()) {
		float shot_x = m_state->getPlayerRight()->getShotInstance()->getPosX();
		float shot_y = m_state->getPlayerRight()->getShotInstance()->getPosY();
		if (shot_y >= getGroundLevel(shot_x)) {
			m_state->getPlayerRight()->getShotInstance()->reset();

			graphics::playSound(m_state->getFullAssetPath("ground-collision.wav"), 0.5f);
			return;
		}
	}
}
