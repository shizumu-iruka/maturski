#pragma once
#include "SFML/Graphics/Sprite.hpp"
#include "SFML/System/Time.hpp"

class SpriteAnimator {
public:
	SpriteAnimator(sf::Sprite& sprite, int frameWidth, 
		int frameHeight, int framesPerDirection, float frameTime);
	enum Direction { Down, Left, Right, Up };
	void update(sf::Time dt);
	void setDirection(Direction newDirection);
	int getFrameWidth() const;
	int getFrameHeight() const;
	void setDefaultTexture();

private:
	bool shouldAdvanceFrame(sf::Time dt);
	void setUpNextFrame();
	void setUpCurrentFrame();

private:
	sf::Sprite& m_Sprite;

	int m_FrameWidth;
	int m_FrameHeight;
	int m_FramesPerDirection;
	float m_FrameTime;
	int m_CurrentFrame = 0;

	float m_ElapsedTime = 0.f;

    Direction m_CurrentDirection = Direction::Down;
};
