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
	SETCOLOR(m_brush_background.fill_color, 1.0f, 1.0f, 1.0f);
	m_brush_background.fill_opacity = 1.0f;
	m_brush_background.outline_opacity = 0.0f;
	m_brush_background.texture = m_state->getInstance()->getFullAssetPath("background.png");

	for (auto p_gob : m_static_objects)
		if (p_gob) p_gob->init();

	for (auto p_gob : m_dynamic_objects)
		if (p_gob) p_gob->init();

	m_blocks.push_back(Box(2, 2.45, m_block_size_x, m_block_size_y));
	m_blocks.push_back(Box(10, 2.45, m_block_size_x, m_block_size_y));

	m_block_names.push_back("roof.png");
	m_block_names.push_back("roof.png");

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

	for (int i = 0; i < m_blocks.size(); i++)
		drawBlock(i);

	for (auto p_gob : m_static_objects)
		if (p_gob) p_gob->draw();

	for (auto p_gob : m_dynamic_objects)
		if (p_gob) p_gob->draw();
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
	for (auto& box : m_blocks) {
		if (m_state->getPlayerLeft()->getShootingFlag() and m_state->getPlayerLeft()->getShotInstance()->intersect(box))
			m_state->getPlayerLeft()->getShotInstance()->reset();
			break;
	}

	for (auto& box : m_blocks) {
		float offset = 0.0f;
		if (m_state->getIsLeftTurn() and m_state->getPlayerLeft()->intersectSideways(box))
			m_state->getPlayerLeft()->m_pos_x += offset;
			break;
	}

	for (auto& box : m_blocks) {
		float offset = 0.0f;
		if (not m_state->getIsLeftTurn() and m_state->getPlayerRight()->intersectSideways(box))
			m_state->getPlayerRight()->m_pos_x += offset;
			break;
	}
}
