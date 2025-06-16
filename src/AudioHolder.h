#pragma once
#include "ResourceHolder.h"

enum class AudioID { Walking, GameCompletion };

using AudioHolder = ResourceHolder<sf::SoundBuffer, AudioID>;