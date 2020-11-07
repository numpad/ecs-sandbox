#include "sgl/sgl_audio_listener.hpp"

//////////////
//  STATIC  //
//////////////

bool sgl::audio_listener::has_extension(std::string name) {
	return alcIsExtensionPresent(NULL, name.c_str()) == AL_TRUE;
}

std::vector<std::string> sgl::audio_listener::get_available() {
	std::vector<std::string> devices;
	
	if (sgl::audio_listener::has_extension("ALC_ENUMERATION_EXT")) {
		const ALCchar *names = alcGetString(NULL, ALC_DEVICE_SPECIFIER);
		int i = 0;
		while (!(names[i] == '\0' && names[i+1] == '\0')) {
			std::string name{&names[i]};
			i += name.size() + 1;
			devices.push_back(name);
		}
	} else {
		// default device
		const ALCchar *defaultName = alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);
		devices.push_back(std::string{defaultName});	
	}
	
	
	return devices;
}

////////////
// PUBLIC //
////////////

sgl::audio_listener::audio_listener(std::string devicename) {
	m_device = alcOpenDevice(devicename.c_str());
	
	if (m_device) {
		m_devicename = std::string{alcGetString(m_device, ALC_DEVICE_SPECIFIER)};
		m_context = alcCreateContext(m_device, NULL);
		
		if (m_context) {
			if (alcGetCurrentContext() == NULL) {
				activate();
			}
		} else {
			std::cout << "[ERR] OpenAL: could not create context for '" << m_devicename << "'." << std::endl;
		}
	} else {
		m_devicename = "<uninitialized>";
	}
}

sgl::audio_listener::~audio_listener() {
	// set current context to null if this devices context is still current.
	if (alcGetCurrentContext() == m_context) {
		std::cout << "[LOG] OpenAL: context for soon to be deleted device '" << m_devicename << "' is still current, setting to NULL." << std::endl;
		if (!alcMakeContextCurrent(NULL)) {
			std::cout << "[ERR] OpenAL: could not make NULL-context current for '" << m_devicename << "'" << std::endl;
		}
	}
	
	if (!alcCloseDevice(m_device)) {
		std::cout << "[LOG] OpenAL: failed closing device '" << m_devicename << "'." << std::endl;
	}
	
	alcDestroyContext(m_context);
}

void sgl::audio_listener::activate() {
	if (!alcMakeContextCurrent(m_context)) {
		std::cout << "[ERR] OpenAL: could not make context of '" << m_devicename << "' current." << std::endl;
	}
}

void sgl::audio_listener::set_gain(float gain) {
	// TODO: activate or check if active
	alListenerf(AL_GAIN, gain);
}

void sgl::audio_listener::set_position(glm::vec3 pos) {
	// TODO: activate or check if active
	alListener3f(AL_POSITION, pos.x, pos.y, pos.z);
}

void sgl::audio_listener::set_velocity(glm::vec3 vel) {
	// TODO: activate or check if active
	alListener3f(AL_VELOCITY, vel.x, vel.y, vel.z);
}

void sgl::audio_listener::set_orientation(glm::vec3 dir, glm::vec3 up) {
	// TODO: activate or check if active
	float vals[] = {dir.x, dir.y, dir.z, up.x, up.y, up.z};
	alListenerfv(AL_ORIENTATION, vals);
}