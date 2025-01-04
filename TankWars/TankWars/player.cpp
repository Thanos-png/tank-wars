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
		m_pos_y = m_state->getCurrentLevel()->getGroundLevel(m_pos_x) - 0.15f;
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

	// Each sprite covers 5 degrees
	m_sprites.push_back(m_state->getInstance()->getFullAssetPath("tank0.png"));
	m_sprites.push_back(m_state->getInstance()->getFullAssetPath("tank5.png"));
	m_sprites.push_back(m_state->getInstance()->getFullAssetPath("tank10.png"));
	m_sprites.push_back(m_state->getInstance()->getFullAssetPath("tank15.png"));
	m_sprites.push_back(m_state->getInstance()->getFullAssetPath("tank20.png"));
	m_sprites.push_back(m_state->getInstance()->getFullAssetPath("tank25.png"));
	m_sprites.push_back(m_state->getInstance()->getFullAssetPath("tank30.png"));
	m_sprites.push_back(m_state->getInstance()->getFullAssetPath("tank35.png"));
	m_sprites.push_back(m_state->getInstance()->getFullAssetPath("tank40.png"));
	m_sprites.push_back(m_state->getInstance()->getFullAssetPath("tank45.png"));
	m_sprites.push_back(m_state->getInstance()->getFullAssetPath("tank50.png"));
	m_sprites.push_back(m_state->getInstance()->getFullAssetPath("tank-5.png"));
	m_sprites.push_back(m_state->getInstance()->getFullAssetPath("tank-10.png"));
	m_sprites.push_back(m_state->getInstance()->getFullAssetPath("tank-15.png"));
	m_sprites.push_back(m_state->getInstance()->getFullAssetPath("tank-20.png"));
	m_sprites.push_back(m_state->getInstance()->getFullAssetPath("tank-25.png"));
	m_sprites.push_back(m_state->getInstance()->getFullAssetPath("tank-30.png"));
	m_sprites.push_back(m_state->getInstance()->getFullAssetPath("tank-35.png"));
	m_sprites.push_back(m_state->getInstance()->getFullAssetPath("tank-40.png"));
	m_sprites.push_back(m_state->getInstance()->getFullAssetPath("tank-45.png"));
	m_sprites.push_back(m_state->getInstance()->getFullAssetPath("tank-50.png"));
}

void Player::draw()
{
	float x = m_pos_x + m_state->m_global_offset_x;
	float y = m_pos_y + m_state->m_global_offset_y;

	// Calculate the gradient at the player's position
	float gradient = 0.0f;
	if (m_state and m_state->getCurrentLevel()) {
		gradient = m_state->getCurrentLevel()->getGroundGradient(m_pos_x);
	}

	// Convert gradient to degrees
	float angle = atan(gradient) * 180.0f / 3.14159f;

	// Flip the angle for the right player
	if (not m_isLeftPlayer) {
		angle = -angle;
	}

	// Clamp the angle to the range of available sprites
	int spriteIndex = 0;
	if (angle >= -54.9f and angle <= 54.9f) {
		if (angle >= 0.0f) {
			// Positive angles (e.g., 0 to 50 degrees)
			spriteIndex = static_cast<int>(angle / 5);
		}
		else {
			// Small negative angles (-0.01 to -4.99) should map to the 0-degree sprite
			if (angle > -5.0f) {
				spriteIndex = 0;
			}
			else {
				// Larger negative angles (-5 to -50 degrees)
				spriteIndex = static_cast<int>((-angle / 5) + 10);
			}
		}
	}

	// Set the appropriate texture
	m_brush_player.texture = m_sprites[spriteIndex];

	// Flip the tank along the x-axis for the right player
	if (not m_isLeftPlayer)
		graphics::setScale(-1.0f, 1.0f);

	// Draw the tank
	graphics::drawRect(x + 0.05f, y - 0.11f, 1.2 * m_width, 1.2 * m_height, m_brush_player);
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

	// Calculate the gradient at the player's position
	float gradient = 0.0f;
	if (m_state && m_state->getCurrentLevel()) {
		gradient = m_state->getCurrentLevel()->getGroundGradient(m_pos_x);
	}

	// Convert gradient to degrees
	float angle = atan(gradient) * 180.0f / 3.14159f;

	// Don't update the position during firing
	// Or if gradient > 54.5 or gradient < -55.4 the ground is too steep for the tank to drive
	if (not m_shootingFlag) {
		if (graphics::getKeyState(graphics::SCANCODE_A) and m_pos_x > 0.1f and angle <= 54.5f)
			m_pos_x -= velocity * delta_time;
		if (graphics::getKeyState(graphics::SCANCODE_D) and m_pos_x < 33.6f and angle >= -54.5f)
			m_pos_x += velocity * delta_time;
		if (graphics::getKeyState(graphics::SCANCODE_W) and m_pos_x < 33.6f and angle >= -54.5f)
			m_pos_x += velocity * delta_time;
		if (graphics::getKeyState(graphics::SCANCODE_S) and m_pos_x > 0.1f and angle <= 54.5f)
			m_pos_x -= velocity * delta_time;

		if (graphics::getKeyState(graphics::SCANCODE_LEFT) and m_pos_x > 0.1f and angle <= 54.5f)
			m_pos_x -= velocity * delta_time;
		if (graphics::getKeyState(graphics::SCANCODE_RIGHT) and m_pos_x < 33.6f and angle >= -54.5f)
			m_pos_x += velocity * delta_time;
		if (graphics::getKeyState(graphics::SCANCODE_UP) and m_pos_x < 33.6f and angle >= -54.5f)
			m_pos_x += velocity * delta_time;
		if (graphics::getKeyState(graphics::SCANCODE_DOWN) and m_pos_x > 0.1f and angle <= 54.5f)
			m_pos_x -= velocity * delta_time;
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
		m_pos_y = m_state->getCurrentLevel()->getGroundLevel(m_pos_x) - 0.15f;
	}
	else {
		m_pos_y = 3.0f; // Default value if level or state is not properly set
	}

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
