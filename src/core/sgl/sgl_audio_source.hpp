#pragma once

#include <AL/al.h>
#include <AL/alc.h>

#include <glm/glm.hpp>

#include "sgl_audio.hpp"

namespace sgl {
class audio_source {
public:
	audio_source();
	~audio_source();

	void set_buffer(sgl::audio& audio);

	void play();
	void pause();
	void stop();
	void rewind();

	void set_position(glm::vec3 pos);
	void set_velocity(glm::vec3 vel);
	void set_direction(glm::vec3 dir);

	void set_relative(bool relative);
	void set_loop(bool enabled = true);

	void set_pitch(float pitch);
	void set_gain(float gain);
	void set_reference_distance(float dist);
	void set_max_distance(float dist);
	void set_rolloff(float rolloff);
	void set_min_gain(float mingain);
	void set_max_gain(float maxgain);

	void set_seek(float seconds);
	void set_sseek(int samples);
	void set_bseek(int bytes);

	float get_seek();
	int get_sseek();
	int get_bseek();

private:
	ALuint m_source;
};
} // namespace sgl
