#include "sgl/sgl_audio_source.hpp"

////////////
// PUBLIC //
////////////

sgl::audio_source::audio_source() {
	alGenSources(1, &m_source);
}

sgl::audio_source::~audio_source() {
	alDeleteSources(1, &m_source);
}

void sgl::audio_source::set_buffer(sgl::audio& audio) {
	alSourcei(m_source, AL_BUFFER, (ALuint)audio);
}

void sgl::audio_source::play() {
	alSourcePlay(m_source);
}

void sgl::audio_source::pause() {
	alSourcePause(m_source);
}

void sgl::audio_source::stop() {
	alSourceStop(m_source);
}

void sgl::audio_source::rewind() {
	alSourceRewind(m_source);
}

void sgl::audio_source::set_position(glm::vec3 pos) {
	alSource3f(m_source, AL_POSITION, pos.x, pos.y, pos.z);
}

void sgl::audio_source::set_velocity(glm::vec3 vel) {
	alSource3f(m_source, AL_VELOCITY, vel.x, vel.y, vel.z);
}

void sgl::audio_source::set_direction(glm::vec3 dir) {
	alSource3f(m_source, AL_DIRECTION, dir.x, dir.y, dir.z);
}

void sgl::audio_source::set_pitch(float pitch) {
	alSourcef(m_source, AL_PITCH, pitch);
}

void sgl::audio_source::set_gain(float gain) {
	alSourcef(m_source, AL_GAIN, gain);
}

void sgl::audio_source::set_reference_distance(float dist) {
	alSourcef(m_source, AL_REFERENCE_DISTANCE, dist);
}

void sgl::audio_source::set_max_distance(float dist) {
	alSourcef(m_source, AL_MAX_DISTANCE, dist);
}

void sgl::audio_source::set_rolloff(float rolloff) {
	alSourcef(m_source, AL_ROLLOFF_FACTOR, rolloff);
}

void sgl::audio_source::set_min_gain(float mingain) {
	alSourcef(m_source, AL_MIN_GAIN, mingain);
}

void sgl::audio_source::set_max_gain(float maxgain) {
	alSourcef(m_source, AL_MAX_GAIN, maxgain);
}

void sgl::audio_source::set_loop(bool enabled) {
	alSourcei(m_source, AL_LOOPING, enabled ? AL_TRUE : AL_FALSE);
}

void sgl::audio_source::set_relative(bool relative) {
	alSourcei(m_source, AL_SOURCE_RELATIVE, relative ? AL_TRUE : AL_FALSE);
}

void sgl::audio_source::set_seek(float seconds) {
	alSourcef(m_source, AL_SEC_OFFSET, seconds);
}

void sgl::audio_source::set_sseek(int samples) {
	alSourcei(m_source, AL_SAMPLE_OFFSET, samples);
}

void sgl::audio_source::set_bseek(int bytes) {
	alSourcei(m_source, AL_BYTE_OFFSET, bytes);
}

float sgl::audio_source::get_seek() {
	float value;
	alGetSourcef(m_source, AL_SEC_OFFSET, &value);
	return value;
}

int sgl::audio_source::get_sseek() {
	int value;
	alGetSourcei(m_source, AL_SAMPLE_OFFSET, &value);
	return value;
}

int sgl::audio_source::get_bseek() {
	int value;
	alGetSourcei(m_source, AL_BYTE_OFFSET, &value);
	return value;
}
