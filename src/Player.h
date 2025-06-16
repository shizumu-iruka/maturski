#pragma once
#include <SFML/Window/Keyboard.hpp>

#include "SpriteAnimator.h"

class Game;

class Player : public sf::Drawable, public sf::Transformable {
public:
	Player(const sf::Texture& texture, sf::Vector2f position, sf::Vector2f speed);

	void handleInput(sf::Keyboard::Key key, bool isPressed);
	void update(sf::Time dt, Game& game);

	sf::Vector2f getSpriteCenter();
	sf::Vector2f getSpriteSize();

private:
	void setUpSprite(sf::Vector2f pos);
	sf::Vector2f calculateMovementAndDirection();
	bool canMoveTo(sf::Vector2f newPos, Game& game);
	void checkForGameEnd(Game& game);

protected:
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
	sf::Sprite m_Sprite;
	sf::Vector2f m_Speed;

	SpriteAnimator m_SpriteAnimator;
	
	bool m_MovingUp = false;
	bool m_MovingDown = false;
	bool m_MovingLeft = false;
	bool m_MovingRight = false;
	bool m_WasMovingLastFrame = false;
};
