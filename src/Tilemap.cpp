#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Texture.hpp>

#include "Tilemap.h"

void Tilemap::load(const sf::Texture& tileset, sf::Vector2u tileSize,
	const int* level, unsigned int width, unsigned height) {
	m_Tileset = &tileset;

	m_Vertices.setPrimitiveType(sf::PrimitiveType::Triangles);
	m_Vertices.resize(width * height * 6);

    for (unsigned int i = 0; i < width; i++) {
        for (unsigned int j = 0; j < height; j++) {
            const int tileNumber = level[i + j * width];

            const int tu = tileNumber % (m_Tileset->getSize().x / tileSize.x);
            const int tv = tileNumber / (m_Tileset->getSize().x / tileSize.x);

            auto triangles = &m_Vertices[(i + static_cast<size_t>(j) * width) * 6];

            float fx = static_cast<float>(i * tileSize.x);
            float fy = static_cast<float>(j * tileSize.y);
            float fx1 = static_cast<float>((i + 1) * tileSize.x);
            float fy1 = static_cast<float>((j + 1) * tileSize.y);

            float tx = static_cast<float>(tu * tileSize.x);
            float ty = static_cast<float>(tv * tileSize.y);
            float tx1 = static_cast<float>((tu + 1) * tileSize.x);
            float ty1 = static_cast<float>((tv + 1) * tileSize.y);

            triangles[0].position = sf::Vector2f(fx, fy);
            triangles[1].position = sf::Vector2f(fx1, fy);
            triangles[2].position = sf::Vector2f(fx, fy1);
            triangles[3].position = sf::Vector2f(fx, fy1);
            triangles[4].position = sf::Vector2f(fx1, fy);
            triangles[5].position = sf::Vector2f(fx1, fy1);

            triangles[0].texCoords = sf::Vector2f(tx, ty);
            triangles[1].texCoords = sf::Vector2f(tx1, ty);
            triangles[2].texCoords = sf::Vector2f(tx, ty1);
            triangles[3].texCoords = sf::Vector2f(tx, ty1);
            triangles[4].texCoords = sf::Vector2f(tx1, ty);
            triangles[5].texCoords = sf::Vector2f(tx1, ty1);
        }
    }
}

void Tilemap::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    states.transform *= getTransform();
    states.texture = m_Tileset;
    target.draw(m_Vertices, states);
}
