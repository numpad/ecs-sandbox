#pragma once

struct EngineConfig {
	enum class WindowMode {
		WINDOW, BORDERLESS, FULLSCREEN
	};

	int window_width = 800;
	int window_height = 600;
	WindowMode windowMode = WindowMode::WINDOW;

	bool imgui_enabled = true;
};
