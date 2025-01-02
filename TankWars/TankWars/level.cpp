#include "level.h"
#include "gamestate.h"
#include "player.h"
#include "util.h"


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

void Level::init()
{
	setGroundFunction([](float x) -> float { return 2.9f; });

	SETCOLOR(m_brush_background.fill_color, 1.0f, 1.0f, 1.0f);
	m_brush_background.fill_opacity = 1.0f;
	m_brush_background.outline_opacity = 0.0f;
	m_brush_background.texture = m_state->getInstance()->getFullAssetPath("background.png");

	for (auto p_gob : m_static_objects)
		if (p_gob) p_gob->init();

	for (auto p_gob : m_dynamic_objects)
		if (p_gob) p_gob->init();

	Box leftBase = Box(2, 2.45f, m_block_size_x, m_block_size_y);
	leftBase.isLeftBase = true;
	m_blocks.push_back(leftBase);

	Box rightBase = Box(10, 2.45f, m_block_size_x, m_block_size_y);
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
	float w = m_state->getCanvasWidth();
	float h = m_state->getCanvasHeight();

	float offset_x = m_state->m_global_offset_x / 4.0f + w / 2.0f;
	float offset_y = m_state->m_global_offset_y / 4.0f + h / 2.0f;

	// Draw background
	graphics::drawRect(offset_x, offset_y, w, h, m_brush_background);

	// Add clouds and stuff

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
	// graphics::drawRect(2, 2.5, m_block_size_x, m_block_size_y, m_brush_testing);
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

void Level::addBlocks(float m_pos_x, float m_pos_y, float m_width, float m_height)
{
	if (m_state->getIsLeftTurn() and m_state->getPlayerLeft()->getShotInstance()->isActive()) {
		Box leftShield = Box(m_pos_x, m_pos_y, m_width, m_height);
		m_blocks.push_back(leftShield);
		leftShield.isShield = true;
		m_block_names.push_back("shield.png");
	}
	else if (not m_state->getIsLeftTurn() and m_state->getPlayerRight()->getShotInstance()->isActive()) {
		Box rightShield = Box(m_pos_x, m_pos_y, m_width, m_height);
		m_blocks.push_back(rightShield);
		rightShield.isShield = true;
		m_block_names.push_back("shield.png");
	}
}

void Level::removeBlocks()
{
}

std::vector<Box>& Level::getBlocks()
{
	return m_blocks;
}

void Level::setGroundFunction(std::function<float(float)> func)
{
	m_ground_function = func;
}

float Level::getGroundLevel(float x)
{
	if (m_ground_function) {
		return m_ground_function(x);
	}
	return 0.0f;  // Default ground level if no function is set
}

void Level::drawBlock(int i)
{
	Box& box = m_blocks[i];
	float x = box.m_pos_x + m_state->m_global_offset_x;
	float y = box.m_pos_y + m_state->m_global_offset_y;
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

			// Destroy shield
			if (adjustedBox.isShield) {
				m_state->getPlayerLeft()->getShieldInstance()->reset();
			}

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

			// Destroy shield
			if (adjustedBox.isShield) {
				m_state->getPlayerLeft()->getShieldInstance()->reset();
			}

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
		float left_shield_x = m_state->getPlayerLeft()->getShieldInstance()->getPosX();
		float left_shield_y = m_state->getPlayerLeft()->getShieldInstance()->getPosY();
		Box leftShield = Box(left_shield_x, left_shield_y, m_shield_size_x, m_shield_size_y);

		float right_shield_x = m_state->getPlayerRight()->getShieldInstance()->getPosX();
		float right_shield_y = m_state->getPlayerRight()->getShieldInstance()->getPosY();
		Box rightShield = Box(right_shield_x, right_shield_y, m_shield_size_x, m_shield_size_y);

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
		if (offset = m_state->getPlayerLeft()->intersectSideways(leftShield))
			m_state->getPlayerLeft()->m_pos_x += offset;
		if (offset = m_state->getPlayerLeft()->intersectSideways(rightShield))
			m_state->getPlayerLeft()->m_pos_x += offset;
		if (offset = m_state->getPlayerRight()->intersectSideways(leftShield))
			m_state->getPlayerRight()->m_pos_x += offset;
		if (offset = m_state->getPlayerRight()->intersectSideways(rightShield))
			m_state->getPlayerRight()->m_pos_x += offset;
		m_state->m_global_offset_x += offset;
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

	// Check if the shot collides with the enemy player
	float player_x = m_state->getPlayerRight()->getPosX();
	float player_y = m_state->getPlayerRight()->getPosY();
	float player_width = m_state->getPlayerRight()->getWidth();
	float player_height = m_state->getPlayerRight()->getHeight();
	Box player_right = Box(player_x, player_y, player_width, player_height);
	if (m_state->getIsLeftTurn() and m_state->getPlayerLeft()->getShotInstance()->isActive() and
		m_state->getPlayerLeft()->getShotInstance()->intersect(player_right) and m_state->getPlayerLeft()->getShootingFlag()) {

		m_state->getPlayerLeft()->getShotInstance()->reset();
		m_score_left += 1;

		graphics::playSound(m_state->getFullAssetPath("ground-collision.wav"), 0.5f);
		return;
	}

	player_x = m_state->getPlayerLeft()->getPosX();
	player_y = m_state->getPlayerLeft()->getPosY();
	player_width = m_state->getPlayerLeft()->getWidth();
	player_height = m_state->getPlayerLeft()->getHeight();
	Box player_left = Box(player_x, player_y, player_width, player_height);
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

		float x = shot_x + m_state->m_global_offset_x;
		float y = shot_y + m_state->m_global_offset_y;
		if (y >= getGroundLevel(x)) {
			m_state->getPlayerLeft()->getShotInstance()->reset();

			graphics::playSound(m_state->getFullAssetPath("ground-collision.wav"), 0.5f);
			return;
		}
	}

	if (m_state->getPlayerRight()->getShotInstance()->isActive() and m_state->getPlayerRight()->getShootingFlag()) {
		float shot_x = m_state->getPlayerRight()->getShotInstance()->getPosX();
		float shot_y = m_state->getPlayerRight()->getShotInstance()->getPosY();

		float x = shot_x + m_state->m_global_offset_x;
		float y = shot_y + m_state->m_global_offset_y;
		if (y >= getGroundLevel(x)) {
			m_state->getPlayerRight()->getShotInstance()->reset();

			graphics::playSound(m_state->getFullAssetPath("ground-collision.wav"), 0.5f);
			return;
		}
	}
}
