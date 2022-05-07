#pragma once

#include <vector>
#include <string>
#include <filesystem>

#include <fmt/core.h>
#include <yoga/Yoga.h>
#include <sgl/sgl_texture.hpp>

#include <Engine/Engine.hpp>
#include <Engine/IScene.hpp>
#include <RenderObject/Camera.hpp>
#include <scenes/ui/MainMenuScene.hpp>

class SpritePreviewScene : public IScene {
public:

	bool onCreate() override;
	void onDestroy() override;
	void onUpdate(float dt) override;
	void onRender() override;

private:
	Camera *m_camera = nullptr;
	double m_elapsedTime = 0.0;

	sgl::shader *m_imageShader, *m_previewShader;
	sgl::texture *m_texture, *m_texCoords;
	std::vector<std::string> m_files;
	GLuint m_vao, m_vbo;
	void createLogo();

	void reload();

};
