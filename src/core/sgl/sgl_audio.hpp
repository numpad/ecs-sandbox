#pragma once

#include <AL/al.h>
#include <AL/alc.h>

namespace sgl {

class audio {
public:
	enum class format : ALenum {
		mono8 = AL_FORMAT_MONO8,
		mono16 = AL_FORMAT_MONO16,
		stereo8 = AL_FORMAT_STEREO8,
		stereo16 = AL_FORMAT_STEREO16
	};

public:
	audio();
	~audio();

	void load(format loaderformat, ALsizei frequency, ALsizei size, void* data);

	operator ALuint() const {
		return m_buffer;
	}

private:
	ALuint m_buffer;
};
} // namespace sgl
