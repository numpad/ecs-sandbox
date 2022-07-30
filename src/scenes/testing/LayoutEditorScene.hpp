#pragma once

#include <vector>
#include <string>
#include <filesystem>

#include <yoga/Yoga.h>

#include "UI/Layout.hpp"
#include "Engine/IScene.hpp"
#include "RenderObject/Camera.hpp"
#include "scenes/ui/MainMenuScene.hpp"

class LayoutEditorScene : public IScene {
public:
	bool onCreate() override;
	void onDestroy() override;
	void onUpdate(float dt) override;
	void onRender() override;

private:
	Camera* m_camera = nullptr;
	double m_elapsedTime = 0.0;

	std::vector<std::string> m_files;

	Layout m_ui;
	YGNodeRef m_selectedNode = nullptr;
	void updateFiles();
	void loadLayout(const std::string& filename);
};
