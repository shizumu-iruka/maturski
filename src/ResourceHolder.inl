#pragma once
#include "ResourceHolder.h"

template<typename Resource, typename Identifier>
inline void ResourceHolder<Resource, Identifier>::load(Identifier id, const std::string& filename) {
	std::unique_ptr<Resource> resource(new Resource());

    if constexpr (std::is_same<Resource, sf::Texture>::value || 
            std::is_same<Resource, sf::SoundBuffer>::value) {
       if (!resource->loadFromFile(filename)) {
            throw std::runtime_error("Failed to load file: " + filename);
       }
    }
    else if constexpr (std::is_same<Resource, sf::Font>::value ||
            std::is_same<Resource, sf::Music>::value) {
        if (!resource->openFromFile(filename)) {
            throw std::runtime_error("Failed to load file: " + filename);
        }
    }
    else if constexpr (std::is_same<Resource, sf::Shader>::value) {
        if (!resource->loadFromFile(filename,sf::Shader::Type::Fragment)) {
            throw std::runtime_error("Failed to load file: " + filename);
        }
    }
    else {
        static_assert(false, "Unsupported resource type");
    }

	auto insert = m_ResourceMap.insert(std::make_pair(id, std::move(resource)));
	assert(insert.second);
}
template<typename Resource, typename Identifier>
inline Resource& ResourceHolder<Resource, Identifier>::get(Identifier id) {
	return *m_ResourceMap.at(id);
}

template<typename Resource, typename Identifier>
inline const Resource& ResourceHolder<Resource, Identifier>::get(Identifier id) const {
	return *m_ResourceMap.at(id);
}
