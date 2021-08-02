#pragma once

#include <sgl/sgl_texture.hpp>

#include <Engine/Engine.hpp>
#include <Engine/IScene.hpp>
#include <RenderObject/Camera.hpp>

#include <scenes/ui/MainMenuScene.hpp>

class SplashScreenScene : public IScene {
public:

	bool onCreate() override;
	void onDestroy() override;
	void onUpdate(float dt) override;
	void onRender() override;

private:
	Camera *m_camera = nullptr;
	double m_elapsedTime = 0.0;

	sgl::texture *m_logoTexture = nullptr;
	sgl::shader *m_logoShader;

	GLuint m_vao, m_vbo;
	void createLogo();

};
