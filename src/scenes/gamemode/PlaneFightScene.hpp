#pragma once

#include "Engine/IScene.hpp"

class PlaneFightScene : public IScene {
public:
	bool onCreate() override;
	void onDestroy() override;
	void onUpdate(float dt) override;
	void onRender() override;

private:
	entt::registry m_registry;
	Camera* m_camera;
};
