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
}

void Level::draw()
{
	float w = m_state->getCanvasWidth();
	float h = m_state->getCanvasHeight();

	float offset_x = m_state->m_global_offset_x + w / 2.0f;
	float offset_y = m_state->m_global_offset_y + h / 2.0f;

	// Draw background
	graphics::drawRect(offset_x, offset_y, w, h, m_brush_background);

	// Draw Players
	if (m_state->getPlayer1()->isActive())
		m_state->getPlayer1()->draw();
	if (m_state->getPlayer2()->isActive())
		m_state->getPlayer2()->draw();

	for (auto p_gob : m_static_objects)
		if (p_gob) p_gob->draw();

	for (auto p_gob : m_dynamic_objects)
		if (p_gob) p_gob->draw();
}

void Level::update(float dt)
{
	if (m_state->getPlayer1()->isActive())
		m_state->getPlayer1()->update(dt);
	if (m_state->getPlayer2()->isActive())
		m_state->getPlayer2()->update(dt);

	GameObject::update(dt);
}
