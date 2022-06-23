#pragma once

#include <Engine/Engine.hpp>
#include <Engine/IScene.hpp>

#include <sgl/sgl_texture.hpp>

class TestGameScene : public IScene {
public:
	bool onCreate() override;
	void onDestroy() override;
	void onUpdate(float dt) override;
	void onRender() override;

private:
	std::shared_ptr<Camera> m_camera;
	World* m_world = nullptr;
};
