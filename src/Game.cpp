#include <fstream>
#include <sstream>

#include "Game.h"

Game::Game() : m_Window(sf::VideoMode(m_WindowSize, desktopMode.bitsPerPixel), "Maturski", sf::State::Windowed),
	m_OtherObjects(VIRTUAL_RESOLUTION), m_RenderTexture(VIRTUAL_RESOLUTION),
	m_ScaledSprite(m_RenderTexture.getTexture()) {
	setUpWindowSprite();
	loadFiles();
	setUpMusic();
	setUpSFX();
	loadMap(pathToMap);
	initMap();
	initPlayer();
	initShaders();
	setUpText();

}

void Game::run() {
	playBGMusic();

	sf::Clock clock;
	sf::Time timeSinceLastUpdate = sf::Time::Zero;

	while (m_Window.isOpen()) {
		processEvents();
		timeSinceLastUpdate += clock.restart();

		while (timeSinceLastUpdate > m_TimePerFrame) {
			timeSinceLastUpdate -= m_TimePerFrame;
			processEvents();
			update(m_TimePerFrame);
		}
		render();
	}
}

sf::Vector2u Game::getFieldPos(sf::Vector2f objectPos) {
	return sf::Vector2u{ (unsigned int)objectPos.x / tileSize.x, (unsigned int)objectPos.y / tileSize.y };
}

void Game::setGameState(GameState state) {
	m_GameState = state;
}

GameState Game::getGameState() const {
	return m_GameState;
}

void Game::switchSoundState(AudioID id) {
	switch (id) {
	case (AudioID::Walking):
		if (m_WalkingSound->getStatus() != sf::Sound::Status::Playing) {
			m_WalkingSound->play();
		}
		else m_WalkingSound->stop();
		break;
	case (AudioID::GameCompletion): m_GameCompleteSound->play();
		break;
	}
}

sf::Sound::Status Game::getSoundState(AudioID id) {
	switch (id) {
	case AudioID::Walking: return m_WalkingSound->getStatus();
		break;
	case AudioID::GameCompletion: return m_GameCompleteSound->getStatus();
	default:
		break;
	}
	throw std::runtime_error("Can't find sound");
}

void Game::stopMusic() {
	m_MusicHolder.get(MusicID::TimeRunningOut).stop();
}

void Game::update(sf::Time dt) {
	if (m_GameState == GameState::Playing) {
		m_OtherObjects.update(dt, *this);
	}
}

void Game::render() {
	m_RenderTexture.clear();

	switch (m_GameState) {
	case GameState::Playing:
		m_RenderTexture.draw(m_OtherObjects);
		break;

	case GameState::Finished:
		if (m_ObjectiveClock.isRunning()) {
			m_EscapeTime = m_ObjectiveClock.reset();
		}

		if (!m_EndSet) {
			std::ostringstream stream;
			stream << std::fixed << std::setprecision(2) << m_EscapeTime.asSeconds() << "sec";

			std::string message = m_EndText->getString() + stream.str();
			m_EndText->setString(message);

			m_EndSet = true;
		}

		m_RenderTexture.draw(*m_EndText);
		break;
	}

	m_RenderTexture.display();

	m_Window.clear();
	m_Window.draw(m_ScaledSprite);
	m_Window.display();
}

void Game::processEvents() {
	while (const std::optional event = m_Window.pollEvent()) {
		if (event->is<sf::Event::KeyPressed>()) {
			auto keycode = event->getIf<sf::Event::KeyPressed>()->code;
			if (keycode == sf::Keyboard::Key::F11) {
				recreateWindow();
			}
			if (keycode == sf::Keyboard::Key::Escape) {
				m_Window.close();
			}
			handleInput(event->getIf<sf::Event::KeyPressed>()->code, true);
		}
		else if (event->is<sf::Event::KeyReleased>()) {
			handleInput(event->getIf<sf::Event::KeyReleased>()->code, false);
		}
		else if (event->is<sf::Event::Resized>()) {
			recreateWindow();
		}
		else if (event->is<sf::Event::Closed>()) {
			m_Window.close();
		}
	}
}

void Game::handleInput(sf::Keyboard::Key key, bool isPressed) {
	m_OtherObjects.handleInput(key, isPressed);
}

void Game::loadMap(const std::string& filename) {
  std::ifstream mapfile(filename);
  if (!mapfile.is_open()) {
      throw std::runtime_error("Failed to open map file: " + filename);
  }
  m_Level.fill(0);

  std::string line;
  unsigned int index = 0;
  bool foundDoorTile = false, foundPlayerTile = false;

  auto getCurrentTile = [&]() -> sf::Vector2u {
	  return sf::Vector2u{ index % tileWidth , index / tileWidth };
	  };

  while (std::getline(mapfile, line) && index < m_Level.size()) {
      std::istringstream iss(line);
      std::string tile;
      while (iss >> tile && index < m_Level.size()) {
		  if (!foundDoorTile && tile == "dor") {
			  doorSquare = getCurrentTile();
			  foundDoorTile = true;
		  }
		  else if (!foundPlayerTile && tile == "pla") {
			  playerSquare = getCurrentTile();
			  foundPlayerTile = true;
		  }
          if (TileMapper.find(tile) != TileMapper.end()) {
              m_Level[index++] = TileMapper[tile];
          }
		  else {
			  m_Level[index++] = TileMapper["key"];
		  }
      }
  }
  mapfile.close();
}

const unsigned int Game::getScale() const {
	return std::min
	(desktopMode.size.x / VIRTUAL_RESOLUTION.x, desktopMode.size.y / VIRTUAL_RESOLUTION.y);
}

void Game::recreateWindow() {
	m_Window.setVisible(false);
	if (!m_IsFullscreen) {
		m_Window.create(sf::VideoMode(desktopMode.size, desktopMode.bitsPerPixel), "Maturski", sf::Style::None);
	}
	else {
		m_Window.create(sf::VideoMode(m_WindowSize), "Maturski", sf::State::Windowed);
	}

	m_Window.setVisible(true);
	m_IsFullscreen = !m_IsFullscreen;
}

void Game::setUpWindowSprite() {
	m_ScaledSprite.setTexture(m_RenderTexture.getTexture(), true);
	m_ScaledSprite.setScale({ static_cast<float>(getScale()), static_cast<float>(getScale()) });
	m_ScaledSprite.setPosition({ 0.f,0.f });
}

void Game::setUpText() {
	m_EndText = std::make_unique<sf::Text>(m_FontHolder.get(FontID::JetBrainsMono));
	m_EndText->setString("You escaped!\nTime:");

	auto textRect = m_EndText->getGlobalBounds();
	m_EndText->setPosition(
		{ VIRTUAL_RESOLUTION.x / 2.f - textRect.size.x / 2.f,
		VIRTUAL_RESOLUTION.y / 2.f - textRect.size.y / 2.f });

	m_EndText->setFillColor(sf::Color(93, 201, 109));
	m_EndText->setOutlineThickness(2.f);
	m_EndText->setOutlineColor(sf::Color(28, 176, 186));
}

void Game::loadFiles() {
	m_TextureHolder.load(TextureID::PlayerSheet, "resources/textures/player.png");
	m_TextureHolder.load(TextureID::Tilemap, "resources/textures/Dungeon_Tileset.png");

	m_FontHolder.load(FontID::JetBrainsMono, "resources/fonts/JetBrainsMono-Medium.ttf");

	m_AudioHolder.load(AudioID::Walking, "resources/sfx/player/walking.wav");
	m_AudioHolder.load(AudioID::GameCompletion, "resources/sfx/game/game_complete.wav");

	m_MusicHolder.load(MusicID::TimeRunningOut, "resources/music/background_music.ogg");

	m_ShaderHolder.load(ShaderID::Light, "resources/shaders/lightmask.frag");
}

void Game::setUpMusic() {
	m_MusicHolder.get(MusicID::TimeRunningOut).setVolume(75.f);
	m_MusicHolder.get(MusicID::TimeRunningOut).setLooping(true);
}

void Game::setUpSFX() {
	m_WalkingSound = std::make_unique<sf::Sound>(m_AudioHolder.get(AudioID::Walking));
	m_WalkingSound->setLooping(true);
	m_GameCompleteSound = std::make_unique<sf::Sound>(m_AudioHolder.get(AudioID::GameCompletion));
}

void Game::initMap() {
	m_OtherObjects.initMap(m_TextureHolder.get(TextureID::Tilemap), tileSize, m_Level.data(), tileWidth, tileHeight);
}

void Game::initPlayer() {
	sf::Vector2f playerPos = { static_cast<float>(tileSize.x * playerSquare.x),
		static_cast<float>(tileSize.y * playerSquare.y) };

	m_OtherObjects.initPlayer(m_TextureHolder.get(TextureID::PlayerSheet), playerPos, playerSpeed);
}

void Game::initShaders() {
	m_OtherObjects.initShader(m_ShaderHolder.get(ShaderID::Light));
}

void Game::playBGMusic() {
	m_MusicHolder.get(MusicID::TimeRunningOut).play();
}

bool Game::isWalkableField(sf::Vector2u fieldPos) {
	auto fieldValue = (fieldPos.y * tileWidth) + fieldPos.x;
	for (const auto& [stringCode,value] : TileMapper) {
		if (m_Level[fieldValue] != value) continue;

		//is a walking tile, door or player
		return stringCode[1] == 't' || stringCode == "dor" || stringCode == "pla";
	}

	throw std::runtime_error("Specified field not found");
}

sf::Vector2u Game::getDoorSquare() const {
	return doorSquare;
}