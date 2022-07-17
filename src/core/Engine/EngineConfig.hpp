#pragma once

struct EngineConfig {
	enum class WindowMode {
		WINDOW,
		BORDERLESS,
		FULLSCREEN
	};

	// initialization settings
	int window_width = 800;
	int window_height = 600;
	WindowMode windowMode = WindowMode::WINDOW;

	// graphics settings
	float particle_amount = 1.f;

	bool imgui_enabled = false;
};
