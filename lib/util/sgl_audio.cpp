#include "sgl_audio.hpp"
#include <iostream>

sgl::audio::audio() {
	alGenBuffers(1, &m_buffer);
}

sgl::audio::~audio() {
	alDeleteBuffers(1, &m_buffer);
	std::cout << "--- audio destructed" << std::endl;
}

void sgl::audio::load(format loaderformat, ALsizei frequency, ALsizei size, void *data) {
	alBufferData(m_buffer, static_cast<ALenum>(loaderformat), data, size, frequency);
}
