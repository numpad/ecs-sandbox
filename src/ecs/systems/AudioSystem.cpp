#include <ecs/systems/AudioSystem.hpp>

AudioSystem::AudioSystem(entt::registry &registry)
	: BaseUpdateSystem(registry)
{
	registry.ctx<entt::dispatcher>().sink<PlaySoundEvent>().connect<&AudioSystem::play_sound>(*this);

	// list available output devices
	auto audio_outputs = sgl::audio_listener::get_available();
	std::cout << "Listing available audio output devices." << std::endl;
	for (auto out : audio_outputs) {
		std::cout << " * " << out << std::endl;
	}
		
	unsigned int channels;
	unsigned int sampleRate;
	drwav_uint64 totalPCMFrameCount;
	short *pSampleData = drwav_open_file_and_read_pcm_frames_s16("res/audio/sfx/ouch.wav", &channels, &sampleRate, &totalPCMFrameCount, NULL);
	if (pSampleData == NULL) {
		std::cout << "[ERR] DRWAV: Could not read file." << std::endl;
	}
	
	m_deathsound.load(sgl::audio::format::stereo16, sampleRate, totalPCMFrameCount * channels * sizeof(short), pSampleData);
	drwav_free(pSampleData, nullptr);
	
	for (size_t i = 0; i < 10; ++i) {
		m_sources.emplace_back(new sgl::audio_source());
	}
}

AudioSystem::~AudioSystem() {
	registry.ctx<entt::dispatcher>().sink<PlaySoundEvent>().disconnect<&AudioSystem::play_sound>(*this);
}

void AudioSystem::update() {

}

void AudioSystem::play_sound(const PlaySoundEvent &event) {
	sgl::audio_source &source = *m_sources[m_last_used_source];

	source.set_buffer(m_deathsound);
	source.set_pitch(event.pitch);
	source.play();

	// TODO: naive pooling, replace with smarter solution
	m_last_used_source = (m_last_used_source + 1) % m_sources.size();
}
