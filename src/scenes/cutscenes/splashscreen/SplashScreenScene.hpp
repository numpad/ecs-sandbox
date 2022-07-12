#pragma once

#include "sgl/sgl_texture.hpp"
#include "Engine/IScene.hpp"
#include "RenderObject/Camera.hpp"
#include "UI/Layout.hpp"

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
	Layout m_ui;

	void onKeyInput(const TextInputEvent& event);
};
