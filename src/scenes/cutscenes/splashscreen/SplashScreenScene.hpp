#pragma once

#include <yoga/Yoga.h>
#include <sgl/sgl_texture.hpp>

#include "Engine/Engine.hpp"
#include "Engine/IScene.hpp"
#include "RenderObject/Camera.hpp"
#include "UI/Layout.hpp"
#include "UI/ImageWidget.hpp"
#include "scenes/ui/MainMenuScene.hpp"

class SplashScreenScene : public IScene {
public:
	bool onCreate() override;
	void onDestroy() override;
	void onUpdate(float dt) override;
	void onRender() override;

private:
	Camera* m_camera = nullptr;
	double m_elapsedTime = 0.0;

	sgl::texture* m_logoTexture = nullptr;
	sgl::texture* m_profileTexture = nullptr;
	YGNodeRef m_layout;
	Layout m_ui;

	void onKeyInput(const TextInputEvent& event);
};
