#include "scenes/gamemode/MiniRaftScene.hpp"

bool MiniRaftScene::onCreate() {
	m_registry.set<entt::dispatcher>();
	
	m_camera = new Camera(glm::vec3(0.0f, 0.0f, 1.0f));
	m_camera->setTarget(glm::vec3(0.0f));

	return true;
}

void MiniRaftScene::onDestroy() {
	delete m_camera;
}

void MiniRaftScene::onUpdate(float dt) {
}

void MiniRaftScene::onRender() {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


}
