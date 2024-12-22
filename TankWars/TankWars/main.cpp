#include "sgg/graphics.h"
#include "util.h"
#include <string>

struct GameState
{
	std::string m_asset_path = "assets\\";
	float m_canvas_width = 12.0f;
	float m_canvas_height = 6.0f;

	graphics::Brush m_brush_background;
	graphics::Brush m_brush_player;

	float m_player_pos_x = m_canvas_width / 2.0f;
	float m_player_pos_y = m_canvas_height / 2.0f;
	float m_player_size_x = 1.0f;
	float m_player_size_y = 1.0f;
	float m_cannon_degrees = 0.0f;

	// float m_global_offset_x = 0.0f;
	// float m_global_offset_y = 0.0f;
};

GameState state;

void init()
{
	SETCOLOR(state.m_brush_player.fill_color, 1.0f, 1.0f, 1.0f);
	state.m_brush_player.outline_opacity = 0.0f;

	SETCOLOR(state.m_brush_background.fill_color, 1.0f, 1.0f, 1.0f);
	state.m_brush_background.fill_opacity = 1.0f;
	state.m_brush_background.outline_opacity = 0.0f;
	state.m_brush_background.texture = state.m_asset_path + "background.png";
}

void draw()
{
	graphics::drawRect(state.m_canvas_width / 2.0f, state.m_canvas_height / 2.0f, state.m_canvas_width, state.m_canvas_height, state.m_brush_background);

	graphics::drawRect(state.m_player_pos_x, state.m_player_pos_y, state.m_player_size_x, state.m_player_size_y, state.m_brush_player);
}

void update(float dt)
{
	// float p = 1.0f + fabs(cos(graphics::getGlobalTime() / 2000.0f)) * 0.8f;
	// SETCOLOR(state.m_brush_background.fill_color, 2.4f * p, 0.06f * p, 0.06f * p);

	float delta_time = dt / 1000.0f;
	const float velocity = 10.0f;

	if (graphics::getKeyState(graphics::SCANCODE_A))
		state.m_player_pos_x -= velocity * delta_time;
	if (graphics::getKeyState(graphics::SCANCODE_D))
		state.m_player_pos_x += velocity * delta_time;
	if (graphics::getKeyState(graphics::SCANCODE_W) and state.m_cannon_degrees < 90.0f)
		state.m_cannon_degrees += velocity * delta_time;
	if (graphics::getKeyState(graphics::SCANCODE_S) and state.m_cannon_degrees > 0.0f)
		state.m_cannon_degrees -= velocity * delta_time;

	// state.m_global_offset_x = state.m_canvas_width / 2.0f - state.m_player_pos_x;
	// state.m_global_offset_y = state.m_canvas_height / 2.0f - state.m_player_pos_y;
}

int main(int argc, char** argv)
{
	graphics::createWindow(1200, 600, "Tank Wars");

	init();

	graphics::setDrawFunction(draw);
	graphics::setUpdateFunction(update);

	graphics::setCanvasSize(state.m_canvas_width, state.m_canvas_height);
	graphics::setCanvasScaleMode(graphics::CANVAS_SCALE_FIT);

	graphics::startMessageLoop();

	return 0;
}
