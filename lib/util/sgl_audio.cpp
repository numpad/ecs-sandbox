#include "sgl_audio.hpp"

sgl::audio::audio() {
	alGenBuffers(1, &m_buffer);
}

sgl::audio::~audio() {
	alDeleteBuffers(1, &m_buffer);
}

void sgl::audio::load(format loaderformat, ALsizei frequency, ALsizei size, void *data) {
	alBufferData(m_buffer, static_cast<ALenum>(loaderformat), data, size, frequency);
}
