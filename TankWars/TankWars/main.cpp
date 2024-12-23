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
	float m_player_size_x = 0.78f;  // x is 2.62 times the y
	float m_player_size_y = 0.3f;
	float m_cannon_degrees = 0.0f;

	// float m_global_offset_x = 0.0f;
	// float m_global_offset_y = 0.0f;
};

GameState global_state;

void init()
{
	SETCOLOR(global_state.m_brush_background.fill_color, 1.0f, 1.0f, 1.0f);
	global_state.m_brush_background.fill_opacity = 1.0f;
	global_state.m_brush_background.outline_opacity = 0.0f;
	global_state.m_brush_background.texture = global_state.m_asset_path + "background.png";

	SETCOLOR(global_state.m_brush_player.fill_color, 1.0f, 1.0f, 1.0f);
	SETCOLOR(global_state.m_brush_player.outline_color, 1.0f, 0.3f, 0.1f);
	global_state.m_brush_player.fill_opacity = 1.0f;
	global_state.m_brush_player.outline_opacity = 0.0f;
	global_state.m_brush_player.texture = global_state.m_asset_path + "tank_left.png";
}

void draw()
{
	graphics::drawRect(global_state.m_canvas_width / 2.0f, global_state.m_canvas_height / 2.0f, global_state.m_canvas_width, global_state.m_canvas_height, global_state.m_brush_background);

	graphics::drawRect(global_state.m_player_pos_x, global_state.m_player_pos_y, global_state.m_player_size_x, global_state.m_player_size_y, global_state.m_brush_player);
}

void update(float dt)
{
	// float p = 1.0f + fabs(cos(graphics::getGlobalTime() / 2000.0f)) * 0.8f;
	// SETCOLOR(global_state.m_brush_background.fill_color, 2.4f * p, 0.06f * p, 0.06f * p);

	float delta_time = dt / 1000.0f;
	const float velocity = 0.8f;

	if (graphics::getKeyState(graphics::SCANCODE_A))
		global_state.m_player_pos_x -= velocity * delta_time;
	if (graphics::getKeyState(graphics::SCANCODE_D))
		global_state.m_player_pos_x += velocity * delta_time;
	if (graphics::getKeyState(graphics::SCANCODE_W) and global_state.m_cannon_degrees < 90.0f)
		global_state.m_cannon_degrees += velocity * delta_time;
	if (graphics::getKeyState(graphics::SCANCODE_S) and global_state.m_cannon_degrees > 0.0f)
		global_state.m_cannon_degrees -= velocity * delta_time;

	// global_state.m_global_offset_x = global_state.m_canvas_width / 2.0f - global_state.m_player_pos_x;
	// global_state.m_global_offset_y = global_state.m_canvas_height / 2.0f - global_state.m_player_pos_y;
}

int main(int argc, char** argv)
{
	graphics::createWindow(1200, 600, "Tank Wars");

	init();

	graphics::setDrawFunction(draw);
	graphics::setUpdateFunction(update);

	graphics::setCanvasSize(global_state.m_canvas_width, global_state.m_canvas_height);
	graphics::setCanvasScaleMode(graphics::CANVAS_SCALE_FIT);

	graphics::startMessageLoop();

	return 0;
}
