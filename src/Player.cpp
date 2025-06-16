#include "Player.h"
#include "Game.h"

Player::Player(const sf::Texture& texture, sf::Vector2f position, sf::Vector2f speed)
    : m_Sprite(texture), m_Speed(speed), m_SpriteAnimator(m_Sprite, 16, 16, 3, 0.1f) {
    setUpSprite(position);
}

void Player::handleInput(sf::Keyboard::Key key, bool isPressed) {
    if (isPressed) {
        // Prioritize new key
        m_MovingUp = m_MovingDown = m_MovingLeft = m_MovingRight = false;

        switch (key) {
        case sf::Keyboard::Key::Up: m_MovingUp = true; break;
        case sf::Keyboard::Key::Down:  m_MovingDown = true; break;
        case sf::Keyboard::Key::Left:  m_MovingLeft = true; break;
        case sf::Keyboard::Key::Right: m_MovingRight = true; break;
        default: break;
        }
    }
    else {
        switch (key) {
        case sf::Keyboard::Key::Up:    m_MovingUp = false; break;
        case sf::Keyboard::Key::Down:  m_MovingDown = false; break;
        case sf::Keyboard::Key::Left:  m_MovingLeft = false; break;
        case sf::Keyboard::Key::Right: m_MovingRight = false; break;
        default: break;
        }
    }
}

void Player::update(sf::Time dt, Game& game) {
    sf::Vector2f movement = calculateMovementAndDirection();
    bool isMoving = (movement != sf::Vector2f{ 0.f, 0.f });

    if (isMoving) {
        if (game.getSoundState(AudioID::Walking) != sf::Sound::Status::Playing) {
            game.switchSoundState(AudioID::Walking); //play walking sound
        }

        sf::Vector2f newPos = m_Sprite.getPosition() + movement * dt.asSeconds();
        if (canMoveTo(newPos, game)) {
            m_Sprite.setPosition(newPos);
            checkForGameEnd(game);
        }

        m_SpriteAnimator.update(dt);
    }
    else if (m_WasMovingLastFrame) {
        game.switchSoundState(AudioID::Walking); // stop walking sound
        m_SpriteAnimator.setDefaultTexture();
    }

    m_WasMovingLastFrame = isMoving;
}

sf::Vector2f Player::calculateMovementAndDirection() {
    sf::Vector2f movement{ 0.f, 0.f };

    if (m_MovingUp) {
        movement.y -= m_Speed.y;
        m_SpriteAnimator.setDirection(SpriteAnimator::Up);
    }
    else if (m_MovingDown) {
        movement.y += m_Speed.y;
        m_SpriteAnimator.setDirection(SpriteAnimator::Down);
    }
    else if (m_MovingRight) {
        movement.x += m_Speed.x;
        m_SpriteAnimator.setDirection(SpriteAnimator::Right);
    }
    else if (m_MovingLeft) {
        movement.x -= m_Speed.x;
        m_SpriteAnimator.setDirection(SpriteAnimator::Left);
    }

    return movement;
}

bool Player::canMoveTo(sf::Vector2f newPos, Game& game) {
    auto bounds = m_Sprite.getGlobalBounds();

    const std::vector<std::pair<bool, sf::Vector2f>> directions = {
        {m_MovingRight, {bounds.size.x, bounds.size.y / 2.f}},
        {m_MovingLeft,  {0.f, bounds.size.y / 2.f}},
        {m_MovingDown,  {bounds.size.x / 2.f, bounds.size.y}},
        {m_MovingUp,    {bounds.size.x / 2.f, 0.f}}
    };

    for (const auto& [moving, offset] : directions) {
        if (moving && game.isWalkableField(game.getFieldPos(newPos + offset))) {
            return true;
        }
    }

    return false;
}

void Player::checkForGameEnd(Game& game) {
    sf::Vector2u currentTile = game.getFieldPos(getSpriteCenter());
    if (currentTile == game.getDoorSquare()) {
        game.setGameState(GameState::Finished);
        game.switchSoundState(AudioID::Walking);
        game.stopMusic();
        game.switchSoundState(AudioID::GameCompletion);
    }
}

sf::Vector2f Player::getSpriteCenter() {
    sf::FloatRect bounds = m_Sprite.getGlobalBounds();
    return bounds.getCenter();
}

sf::Vector2f Player::getSpriteSize() {
    return m_Sprite.getGlobalBounds().size;
}

void Player::setUpSprite(sf::Vector2f pos) {
    m_Sprite.setTextureRect(sf::IntRect({ 16, 0 }, { 16, 16 }));
    m_Sprite.setPosition(pos);
}

void Player::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    states.transform *= getTransform();
    target.draw(m_Sprite, states);
}