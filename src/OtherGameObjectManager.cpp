#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Shader.hpp>

#include "OtherGameObjectManager.h"

OtherObjects::OtherObjects(sf::Vector2u windowSize):
	m_DarknessMask(windowSize) {}

void OtherObjects::initMap(const sf::Texture& tilemapTexture, sf::Vector2u tileSize, const int* level, unsigned int width, unsigned height) {
	m_Tilemap.load(tilemapTexture, tileSize , level, width, height);
}

void OtherObjects::initPlayer(const sf::Texture& playerTexture, sf::Vector2f position, sf::Vector2f speed) {
	m_Player = std::make_unique<Player>(playerTexture, position, speed);
}

void OtherObjects::initShader(sf::Shader& shader) {
	m_LightShader = &shader;
}

void OtherObjects::handleInput(sf::Keyboard::Key key, bool isPressed) {
	m_Player->handleInput(key, isPressed);
}

void OtherObjects::update(sf::Time dt,Game& g) {
	m_Player->update(dt, g);
	updateLightMask();
}

void OtherObjects::draw(sf::RenderTarget& target, sf::RenderStates states) const{
	target.draw(m_Tilemap);
	target.draw(sf::Sprite(m_DarknessMask.getTexture()), sf::BlendAlpha);
	target.draw(*m_Player);
}

void OtherObjects::clearTransparentScreen() {
	m_DarknessMask.clear(sf::Color::Transparent);
}

void OtherObjects::updateShader() {
	sf::Vector2f lightCenter = m_Player->getSpriteCenter();
	//inverting y axis to match that of shader
	lightCenter.y = m_DarknessMask.getSize().y - lightCenter.y;
	m_LightShader->setUniform("lightCenter", lightCenter);
	m_LightShader->setUniform("lightRadius", m_Player->getSpriteSize().x * 3.f / 2.f);
}

void OtherObjects::updateLightMask() {
	clearTransparentScreen();
	updateShader();

	sf::RectangleShape fullScreen(sf::Vector2f(m_DarknessMask.getSize()));
	fullScreen.setFillColor(sf::Color::Black);

	m_DarknessMask.draw(fullScreen, m_LightShader);
	m_DarknessMask.display();
}