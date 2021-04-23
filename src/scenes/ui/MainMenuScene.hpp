#pragma once

#include <vector>
#include <string>

#include <Engine/Engine.hpp>
#include <Engine/IScene.hpp>

#include <sgl/sgl_texture.hpp>
#include <scenes/TestGameScene.hpp>
#include <scenes/gamemode/TowerScene.hpp>

class MainMenuScene : public IScene {
public:

	bool onCreate() override;
	void onDestroy() override;
	void onUpdate(float dt) override;
	void onRender() override;

private:
	sgl::texture *m_logo;

	std::vector<std::string> m_modenames = {"Test Game", "Tower Tilt"};
	ssize_t m_modeindex = 1;
};
