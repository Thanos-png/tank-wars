#include "player.h"
#include "gamestate.h"
#include "level.h"
#include "util.h"


Player::Player(std::string name, bool isLeft) : GameObject(name), m_shot(Shot::getInstance()), m_shield(Shield::getInstance()), m_isLeftPlayer(isLeft)
{
}

Player::~Player()
{
	if (m_shot)
		delete m_shot;
	if (m_shield)
		delete m_shield;
}

void Player::init()
{
	if (m_isLeftPlayer) {
		m_pos_x = 6.0f;
	}
	else {
		m_pos_x = 31.5f;
	}
	
	// Calculate m_pos_y based on the ground level at m_pos_x
	if (m_state && m_state->getCurrentLevel()) {
		m_pos_y = m_state->getCurrentLevel()->getGroundLevel(m_pos_x);
	}
	else {
		m_pos_y = 3.0f; // Default value if level or state is not properly set
	}

	// m_state->m_global_offset_x = m_state->getCanvasWidth() / 2.0f - m_pos_x;
	// m_state->m_global_offset_y = m_state->getCanvasHeight() / 2.0f - m_pos_y;

	SETCOLOR(m_brush_player.fill_color, 1.0f, 1.0f, 1.0f);
	m_brush_player.fill_opacity = 1.0f;
	m_brush_player.outline_opacity = 0.0f;
	m_brush_player.texture = m_state->getInstance()->getFullAssetPath("tank0.png");

	m_sprites.push_back(m_state->getInstance()->getFullAssetPath("tank0.png"));
	m_sprites.push_back(m_state->getInstance()->getFullAssetPath("tank10.png"));
	m_sprites.push_back(m_state->getInstance()->getFullAssetPath("tank20.png"));
	m_sprites.push_back(m_state->getInstance()->getFullAssetPath("tank30.png"));
	m_sprites.push_back(m_state->getInstance()->getFullAssetPath("tank40.png"));
	m_sprites.push_back(m_state->getInstance()->getFullAssetPath("tank50.png"));
	m_sprites.push_back(m_state->getInstance()->getFullAssetPath("tank60.png"));
	m_sprites.push_back(m_state->getInstance()->getFullAssetPath("tank70.png"));
	m_sprites.push_back(m_state->getInstance()->getFullAssetPath("tank80.png"));
}

void Player::draw()
{
	float x = m_pos_x + m_state->m_global_offset_x;
	float y = m_pos_y + m_state->m_global_offset_y;

	// Find the correct sprint based on the cannon degrees
	if (m_cannon_degrees < 10.0f) {
		m_brush_player.texture = m_sprites[0];
	}
	else if (m_cannon_degrees < 20.0f) {
		m_brush_player.texture = m_sprites[1];
	}
	else if (m_cannon_degrees < 30.0f) {
		m_brush_player.texture = m_sprites[2];
	}
	else if (m_cannon_degrees < 40.0f) {
		m_brush_player.texture = m_sprites[3];
	}
	else if (m_cannon_degrees < 50.0f) {
		m_brush_player.texture = m_sprites[4];
	}
	else if (m_cannon_degrees < 60.0f) {
		m_brush_player.texture = m_sprites[5];
	}
	else if (m_cannon_degrees < 70.0f) {
		m_brush_player.texture = m_sprites[6];
	}
	else if (m_cannon_degrees < 80.0f) {
		m_brush_player.texture = m_sprites[7];
	}
	else if (m_cannon_degrees >= 80.0f) {
		m_brush_player.texture = m_sprites[8];
	}

	// Flip the tank along the x-axis for the right player
	if (not m_isLeftPlayer)
		graphics::setScale(-1.0f, 1.0f);

	graphics::drawRect(x, y, m_width, m_height, m_brush_player);
	graphics::resetPose();

	// Draw Cannonball
	if (m_shot->isActive() and m_shootingFlag)
		m_shot->draw();

	// Draw Shield
	if (m_shield_amount >= 1 and m_shield->isActive() and m_deployed_shield) {
		m_shield->draw();
	}

	// Debugging
	if (m_state->m_debugging)
		debugDraw();
}

void Player::update(float dt)
{
	// float p = 1.0f + fabs(cos(graphics::getGlobalTime() / 2000.0f)) * 0.8f;
	// SETCOLOR(global_state.m_brush_background.fill_color, 2.4f * p, 0.06f * p, 0.06f * p);

	float delta_time = dt / 1000.0f;
	const float velocity = 0.8f;

	if (not m_shootingFlag) {  // Don't update the position or the cannon degrees during firing
		if (graphics::getKeyState(graphics::SCANCODE_A) and m_pos_x > 0.1f)
			m_pos_x -= velocity * delta_time;
		if (graphics::getKeyState(graphics::SCANCODE_D) and m_pos_x < 33.6f)
			m_pos_x += velocity * delta_time;
		if (graphics::getKeyState(graphics::SCANCODE_W) and m_cannon_degrees < 80.0f)
			m_cannon_degrees += 7 * velocity * delta_time;
		if (graphics::getKeyState(graphics::SCANCODE_S) and m_cannon_degrees > 0.0f)
			m_cannon_degrees -= 7 * velocity * delta_time;

		if (graphics::getKeyState(graphics::SCANCODE_LEFT))
			m_pos_x -= velocity * delta_time;
		if (graphics::getKeyState(graphics::SCANCODE_RIGHT))
			m_pos_x += velocity * delta_time;
		if (graphics::getKeyState(graphics::SCANCODE_UP) and m_cannon_degrees < 80.0f)
			m_cannon_degrees += 7 * velocity * delta_time;
		if (graphics::getKeyState(graphics::SCANCODE_DOWN) and m_cannon_degrees > 0.0f)
			m_cannon_degrees -= 7 * velocity * delta_time;
	}

	// Check to deploy shield
	if (graphics::getKeyState(graphics::SCANCODE_Q)) {
		if (m_shield_amount >= 1 and not m_shield->isActive()) {  // Deploy shield if the player has any and there're inactive
			m_shield->init();  // Reinitialize the shield
			m_deployed_shield = true;
		}
	}

	// Check to fire shot
	if (graphics::getKeyState(graphics::SCANCODE_SPACE)) {
		if (not m_shootingFlag and not m_shot->isActive()) {  // Fire only if the shot is inactive
			m_shot->setCannonDegrees(m_cannon_degrees);
			m_shot->init();  // Reinitialize the shot
			m_shootingFlag = true;

			graphics::playSound(m_state->getFullAssetPath("fire-shot.wav"), 0.5f);
		}
	}

	draw();

	// Update shot
	if (m_shot->isActive() and m_shootingFlag) {
		m_shot->update(dt);
	}

	// Update shield
	if (m_shield->isActive() and m_deployed_shield) {
		m_shield->update(dt);
	}

	// Calculate m_pos_y based on the ground level at m_pos_x
	if (m_state && m_state->getCurrentLevel()) {
		m_pos_y = m_state->getCurrentLevel()->getGroundLevel(m_pos_x);
	}
	else {
		m_pos_y = 3.0f; // Default value if level or state is not properly set
	}

	// m_state->m_global_offset_x = m_pos_x;

	GameObject::update(dt);
}

void Player::debugDraw()
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

void Player::setCannonDegrees(float degrees)
{
	m_cannon_degrees = degrees;
}

float Player::getCannonDegrees()
{
	return m_cannon_degrees;
}

void Player::setShootingFlag(bool flag)
{
	m_shootingFlag = flag;
}

bool Player::getShootingFlag()
{
	return m_shootingFlag;
}

void Player::setDeployedShield(bool deployedShield)
{
	m_deployed_shield = deployedShield;
}

bool Player::getDeployedShield()
{
	return m_deployed_shield;
}

void Player::decreaseShieldAmount()
{
	m_shield_amount -= 1;
}

Shot* Player::getShotInstance()
{
	return m_shot;
}

Shield* Player::getShieldInstance()
{
	return m_shield;
}

float m_cannon_degrees = 0;
