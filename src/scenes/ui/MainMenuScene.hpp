#pragma once

#include <vector>
#include <string>

#include <Engine/Engine.hpp>
#include <Engine/IScene.hpp>

#include <sgl/sgl_texture.hpp>
#include <scenes/testing/TestGameScene.hpp>
#include <scenes/testing/LayoutEditorScene.hpp>
#include <scenes/testing/SpritePreviewScene.hpp>
#include <scenes/gamemode/TowerScene.hpp>

class MainMenuScene : public IScene {
public:
	bool onCreate() override;
	void onDestroy() override;
	void onUpdate(float dt) override;
	void onRender() override;

private:
	sgl::texture* m_logo;

	std::vector<std::string> m_modenames = {"Layout Editor", "Test Game", "Raft", "Tower Tilt", "Sprite Preview"};
	size_t m_modeindex = 2;
};
