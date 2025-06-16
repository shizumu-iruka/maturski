#pragma once
#include <memory>
#include <SFML/Graphics/RenderTexture.hpp>

#include "Player.h"
#include "Tilemap.h"

class OtherObjects : public sf::Drawable {
public:
	OtherObjects(sf::Vector2u windowSize);
	void initMap(const sf::Texture& tilemapTexture, sf::Vector2u tileSize,
		const int* level, unsigned int width, unsigned height);
	void initPlayer(const sf::Texture& playerTexture, sf::Vector2f position, sf::Vector2f speed);
	void initShader(sf::Shader& shader);
	void handleInput(sf::Keyboard::Key key, bool isPressed);
	void update(sf::Time dt, Game& g);

protected:
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
	void clearTransparentScreen();
	void updateShader();
	void updateLightMask();

private:
	Tilemap m_Tilemap;
	std::unique_ptr<Player> m_Player;

	sf::RenderTexture m_DarknessMask;
	sf::Shader* m_LightShader = nullptr;
};