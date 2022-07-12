#pragma once

#include <vector>
#include <string>
#include <iostream>

#include <AL/al.h>
#include <AL/alc.h>

#include <glm/glm.hpp>

namespace sgl {

class audio_listener {
public:
	/**
	 * @brief Checks if an extension is available.
	 * @return True if available, false otherwise.
	 */
	static bool has_extension(std::string name);

	/**
	 * @brief Returns a list of the names of each available audio device.
	 * @return The names of all available audio devices.
	 */
	static std::vector<std::string> get_available();

	/**
	 * @brief Create a output device.
	 * @param devicename The name of the device to open, defaults to default device.
	 * @see sgl::audio_listener#get_available
	 */
	audio_listener(std::string devicename = "\0");
	~audio_listener();

	/**
	 * @brief Makes the devices context current.
	 */
	void activate();

	/**
	 * @brief Sets the listeners master gain.
	 * @param gain The master gain. Should be positive.
	 */
	void set_gain(float gain);
	/**
	 * @brief Sets the listeners position.
	 * @param pos The world space position.
	 */
	void set_position(glm::vec3 pos);
	/**
	 * @brief Sets the listeners velocity.
	 * @param vel The velocity.
	 */
	void set_velocity(glm::vec3 vel);
	/**
	 * @brief Sets the listeners orientation.
	 * @param dir The direction the listener is facing.
	 * @param up Up vector.
	 */
	void set_orientation(glm::vec3 dir, glm::vec3 up = glm::vec3(0.f, 1.f, 0.f));

	std::string get_name() const {
		return m_devicename;
	}
	operator ALCdevice*() const {
		return m_device;
	}

private:
	ALCdevice* m_device;
	ALCcontext* m_context;
	std::string m_devicename;
};
} // namespace sgl
