#pragma once

#include <config.hpp>

#include <vector>
#include <memory>
#include <iostream>

#include <entt/entt.hpp>
#include <dr/dr_wav.h>
#include <util/sgl_audio_listener.hpp>
#include <util/sgl_audio_source.hpp>
#include <util/sgl_audio.hpp>

#include <ecs/systems/BaseUpdateSystem.hpp>
#include <ecs/events.hpp>
#include <Assets/AssetManager.hpp>

class AudioSystem : public BaseUpdateSystem {
public:

	AudioSystem(entt::registry &registry, AssetManager &assetManager);
	~AudioSystem();

	void update();
	
	void play_sound(const PlaySoundEvent &event);

private:
	AssetManager &m_assetManager;
	sgl::audio_listener m_listener;

	// source pool
	std::vector<std::unique_ptr<sgl::audio_source>> m_sources;
	size_t m_last_used_source = 0;

	sgl::audio *m_deathsound;

};
