#pragma once
#include <string>
#include <array>

#include "OtherGameObjectManager.h"
#include "TextureHolder.h"
#include "FontHolder.h"
#include "AudioHolder.h"
#include "MusicHolder.h"
#include "ShaderHolder.h"

enum class GameState {
	Playing, Finished
};

class Game {
public:
	Game();
	void run();
	~Game() = default;

	sf::Vector2u getFieldPos(sf::Vector2f objectPos);
	bool isWalkableField(sf::Vector2u fieldPos);
	sf::Vector2u getDoorSquare() const;

	void setGameState(GameState state);
	GameState getGameState() const;

	void switchSoundState(AudioID id);
	sf::Sound::Status getSoundState(AudioID id);

	void stopMusic();

private:
	void update(sf::Time dt);
	void render();
	void processEvents();
	void handleInput(sf::Keyboard::Key key,bool isPressed);
	void loadMap(const std::string& filename);
	const unsigned int getScale() const;
	void recreateWindow();
	void setUpWindowSprite();
	void setUpText();
	void loadFiles();
	void setUpMusic();
	void setUpSFX();
	void initMap();
	void initPlayer();
	void initShaders();
	void playBGMusic();

private:
	bool m_IsFullscreen = false;
	GameState m_GameState = GameState::Playing;
	sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();

	const sf::Vector2u VIRTUAL_RESOLUTION { 640, 480 };

	sf::Vector2u m_WindowSize = 
		{VIRTUAL_RESOLUTION.x * getScale(),VIRTUAL_RESOLUTION.y * getScale()};
	sf::RenderWindow m_Window;

	OtherObjects m_OtherObjects;
	TextureHolder m_TextureHolder;
	FontHolder m_FontHolder;
	AudioHolder m_AudioHolder;
	MusicHolder m_MusicHolder;
	ShaderHolder m_ShaderHolder;

	sf::RenderTexture m_RenderTexture;
	sf::Sprite m_ScaledSprite;

	std::unique_ptr<sf::Sound> m_WalkingSound, m_GameCompleteSound;

	const sf::Time m_TimePerFrame = sf::seconds(1.f / 60.f);
	sf::Vector2u playerSquare;
	sf::Vector2u doorSquare;
	const sf::Vector2f playerSpeed = { 80.f, 80.f };

	const sf::Vector2u tileSize = { 16u, 16u };
    const unsigned int tileWidth = 40, tileHeight = 30;

	const std::string pathToMap = "resources/files/map.txt";

	std::unordered_map<std::string, unsigned int> TileMapper = {
		 {"uw1", 1 }, {"uw2", 2 }, {"uw3", 3 } , { "uw4" , 4 },
		 {"rw1", 0 }, {"rw2", 10 }, {"rw3", 20 }, {"rw4", 30 }, {"rwi", 40 },
		 {"lw1", 5 }, {"lw2", 15 }, {"lw3", 25 } , {"lw4", 35 }, {"lwi", 45 },
		 {"dw1", 41 }, {"dw2", 42 }, {"dw3", 43 },{"dw4", 44 },
		 {"tt1", 11 }, {"tt2", 12 }, {"tt3", 13 }, {"tt4", 14 },
		 {"lc1", 50 }, {"uc1", 51 }, {"uc2", 52 }, {"rc1", 53}, {"lc2", 54}, {"rc2", 55},
		 {"wt1", 60 }, {"wt2", 61 }, {"wt3", 62 }, {"wt4", 63 },
		 {"wt5", 70 }, {"wt6", 71 }, {"wt7", 72 }, {"wt8", 73 },
		 {"pla", 69 }, {"dor", 37 }, {"key", 99 },
		 {"od1", 49 }, {"od2", 59 }, {"od3", 68 }, {"od4", 77}
    };

	std::array<int, 1200> m_Level;
	sf::Clock m_ObjectiveClock;
	sf::Time m_EscapeTime;
	std::unique_ptr<sf::Text> m_EndText;
	bool m_EndSet = false;
	
    /*
        12,13,14,15 
        22,23,24,25 - prazna polja
        32,33,34,35
    */
};