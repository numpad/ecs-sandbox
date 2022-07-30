#include "scenes/testing/TestGameScene.hpp"

inline glm::vec3 calcCamPos(GLFWwindow* window) {
	// calculate view & projection matrix
	static float angle = 0.0f, angle_vel = 0.0f, angle_acc = 0.3f, cam_dist = 4.5f, cam_y = 3.2f;

	angle_vel *= 0.9f;
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		angle_vel += angle_acc;
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		angle_vel -= angle_acc;
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		cam_dist -= 0.1f;
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		cam_dist += 0.1f;
	angle += angle_vel;
	if (cam_dist < 0.05f)
		cam_dist = 0.05f;

	glm::vec3 campos =
	    glm::vec3(glm::cos(glm::radians(angle)) * cam_dist, cam_y, glm::sin(glm::radians(angle)) * cam_dist);

	return campos;
}

bool TestGameScene::onCreate() {
	m_camera = std::make_shared<Camera>(vec3(0.f));
	m_world = new World(Engine::Instance, m_camera);

	m_world->load();

	return true;
}

void TestGameScene::onDestroy() {
	m_world->destroy();
	delete m_world;
}

void TestGameScene::onUpdate(float dt) {
	// input
	glm::vec2 normalizedMouse = Engine::Instance->window.getNormalizedMousePosition();

	// orbit camera calculations
	static glm::vec3 camtarget(0.0f);
	const float camToTargetSpeed = 0.12f;
	// ease camera to player or origin
	glm::vec3 targetPos;
	if (m_world->getPlayer() != entt::null && m_world->getRegistry().try_get<CPosition>(m_world->getPlayer())) {
		targetPos = m_world->getRegistry().get<CPosition>(m_world->getPlayer()).pos * glm::vec3(1.0f, 0.0f, 1.0f);
	} else {
		targetPos = glm::vec3(0.0f);
	}
	glm::vec3 toTarget = (targetPos - camtarget) * camToTargetSpeed;
	camtarget += toTarget;

	glm::vec3 campos = targetPos + calcCamPos(Engine::Instance->window);
	m_camera->setPos(campos);
	m_camera->setTarget(camtarget);

	// calculate player aim
	entt::entity worldCrosshair = m_world->getCrosshair();

	m3d::ray mcRay = m_world->getCamera()->raycast(normalizedMouse);
	m3d::plane mcFloor(glm::vec3(0.0f, 1.0f, 0.0f));
	glm::vec3 crosspos = m3d::raycast(mcRay, mcFloor);
	bool mouseRightDown = glfwGetMouseButton(Engine::Instance->window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;

	if (m_world->getRegistry().valid(worldCrosshair)) {
		m_world->getRegistry().get<CPosition>(worldCrosshair).pos = crosspos;
	}

	m_world->update();
}

void TestGameScene::onRender() {
	m_world->draw();

	if (ImGui::Begin("luajit")) {
		lua_State* L = m_world->getLuaState();

		constexpr size_t buf_size = 2048;
		static char buf[buf_size] = "";
		ImVec2 win_size = ImGui::GetWindowSize();
		win_size.y -= 58;
		ImGui::InputTextMultiline("##editor", buf, buf_size, win_size);
		if (ImGui::Button("Evaluate")) {
			if (luaL_dostring(L, buf) != 0) {
				std::cerr << "[LUA] Error: " << lua_tostring(L, -1) << std::endl;
				lua_pop(L, 1);
			}
		}
	}
	ImGui::End();
}
