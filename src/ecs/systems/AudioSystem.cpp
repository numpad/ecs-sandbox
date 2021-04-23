#include <ecs/systems/AudioSystem.hpp>

AudioSystem::AudioSystem(entt::registry &registry, AssetManager &assetManager)
	: IUpdateSystem(registry), m_assetManager{assetManager}
{
	registry.ctx<entt::dispatcher>().sink<PlaySoundEvent>().connect<&AudioSystem::play_sound>(*this);

	// list available output devices
	auto audio_outputs = sgl::audio_listener::get_available();
	std::cout << "Listing available audio output devices." << std::endl;
	for (auto out : audio_outputs) {
		std::cout << " * " << out << std::endl;
	}

	for (size_t i = 0; i < 5; ++i) {
		m_sources.emplace_back(new sgl::audio_source);
	}
}

AudioSystem::~AudioSystem() {
	registry.ctx<entt::dispatcher>().sink<PlaySoundEvent>().disconnect<&AudioSystem::play_sound>(*this);
}

void AudioSystem::update(float dt) {

}

void AudioSystem::play_sound(const PlaySoundEvent &event) {
	sgl::audio_source &source = *m_sources[m_last_used_source];

	sgl::audio *sound = m_assetManager.getAudio(event.name);
	if (!sound) {
		#if CFG_DEBUG
			std::cerr << "[WARN] AudioSystem: Tried playing NULL audio, skipping..." << std::endl;
		#endif
		return;
	}

	source.set_buffer(*sound);
	source.set_pitch(event.pitch);
	source.play();

	// TODO: naive pooling, replace with smarter solution
	m_last_used_source = (m_last_used_source + 1) % m_sources.size();
}
