#pragma once

#include <config.hpp>
#include <entt/entt.hpp>
#include <dr/dr_wav.h>

#include <ecs/systems/BaseUpdateSystem.hpp>
#include <util/sgl_audio_listener.hpp>
#include <util/sgl_audio_source.hpp>
#include <util/sgl_audio.hpp>

#include <ecs/events.hpp>

class AudioSystem : public BaseUpdateSystem {
public:

	AudioSystem(entt::registry &registry);
	~AudioSystem();
	
	void update();
	
	void play_sound(const PlaySoundEvent &event);

private:
	sgl::audio_listener m_listener;
	sgl::audio_source m_source;
	sgl::audio m_deathsound;

};
