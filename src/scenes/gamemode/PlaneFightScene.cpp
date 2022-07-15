#include "scenes/gamemode/PlaneFightScene.hpp"

bool PlaneFightScene::onCreate() {
	m_registry.set<entt::dispatcher>();
	
	m_camera = new Camera(glm::vec3(0.0f, 0.0f, 1.0f));
	m_camera->setTarget(glm::vec3(0.0f));

	return true;
}

void PlaneFightScene::onDestroy() {
	delete m_camera;
}

void PlaneFightScene::onUpdate(float dt) {
}

void PlaneFightScene::onRender() {
}
