#pragma once
#include "ResourceHolder.h"

enum class MusicID { TimeRunningOut };

using MusicHolder = ResourceHolder<sf::Music, MusicID>;