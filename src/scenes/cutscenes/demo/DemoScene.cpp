#include "scenes/cutscenes/demo/DemoScene.hpp"

bool DemoScene::onCreate() {
	m_registry.set<entt::dispatcher>();

	m_camera = std::make_shared<Camera>(glm::vec3(5.f, 3.f, 0.f));
	m_camera->setTarget(glm::vec3(0.f));

	Texture* texture = m_assetmanager.getTexture("res/images/sprites/people_oriented.png");
	entt::entity entity = m_registry.create();
	m_registry.emplace<CPosition>(entity, glm::vec3(0.0f));
	m_registry.emplace<CVelocity>(entity, glm::vec3(0.0f));
	m_registry.emplace<CBillboard>(entity, texture, glm::vec2(0.2f));
	m_registry.emplace<CTextureRegion>(entity, 0.0f * 16.0f, 0.0f * 16.0f, 16.0f, 16.0f, 128, 128);
	m_registry.emplace<COrientedTexture>(entity, 8, 0.f);
	m_registry.emplace<CTerrainCollider>(entity, false);
	// m_registry.emplace<CGravity>(entity);

	loadSystems();
	loadTerrainShader();

	m_chunk.polygonize(glm::ivec3(0));

	return true;
}

void DemoScene::onDestroy() {
}

void DemoScene::onUpdate(float dt) {
	static float angle = 0.0f;
	static float dangle = 0.0f;
	if (glfwGetKey(Engine::Instance->window, GLFW_KEY_Q))
		dangle -= 0.035f;
	if (glfwGetKey(Engine::Instance->window, GLFW_KEY_E))
		dangle += 0.035f;
	angle += dangle;
	dangle *= 0.84f;
	m_camera->setPos(glm::vec3(glm::cos(angle) * 4.5f, 2.0f, glm::sin(angle) * 4.5f));
	m_registry.ctx<entt::dispatcher>().update();
	std::for_each(m_updatesystems.begin(), m_updatesystems.end(), [dt](auto& usys) { usys->update(dt); });
}

void DemoScene::onRender() {
	updateTerrainShader();
	m_chunk.draw(m_camera->getProjection(), m_camera->getView());

	std::for_each(m_rendersystems.begin(), m_rendersystems.end(), [](auto& rsys) { rsys->draw(); });
}

/////////////
// PRIVATE //
/////////////

void DemoScene::loadSystems() {
	// register a logging event system
	m_logsystem = std::make_unique<LogSystem>(m_registry);
	m_logsystem->setLogger([](const LogEvent& e) { std::cout << "DemoScene: " << e.text << std::endl; });

	// initialize update and render systems
	auto billboardRenderSystem = std::make_shared<BillboardRenderSystem>(m_registry, m_camera);

	// create update systems
	// m_updatesystems.emplace_back(new DistanceFunctionCollisionSystem(m_registry, m_terrain));
	// m_updatesystems.emplace_back(new CharacterControllerSystem(m_registry, Engine::Instance->window, &m_camera));
	m_updatesystems.emplace_back(new TextureOrientationSystem(m_registry, m_camera));
	m_updatesystems.emplace_back(new GravitySystem(m_registry, 0.000981f));
	m_updatesystems.emplace_back(new PositionUpdateSystem(m_registry));
	m_updatesystems.push_back(billboardRenderSystem);
	m_updatesystems.emplace_back(new DamageSystem(m_registry));
	m_updatesystems.emplace_back(new DespawnSystem(m_registry, -1.f));
	m_updatesystems.emplace_back(new EntityDeleteSystem(m_registry));
	m_updatesystems.emplace_back(new AudioSystem(m_registry, m_assetmanager));

	// and render systems
	// m_rendersystems.emplace_back(new TerrainRenderSystem(m_registry, m_camera, assetManager, chunkedWorld));
	m_rendersystems.emplace_back(new DecalRenderSystem(m_registry, m_camera));
	m_rendersystems.push_back(billboardRenderSystem);
}

void DemoScene::loadTerrainShader() {
}

void DemoScene::updateTerrainShader() {
	glActiveTexture(GL_TEXTURE0);
	m_assetmanager.getTexture("res/images/textures/floor.png")->setWrapMode(Texture::WrapMode::REPEAT);
	m_assetmanager.getTexture("res/images/textures/floor.png")->bind();

	glActiveTexture(GL_TEXTURE1);
	m_assetmanager.getTexture("res/images/textures/wall.png")->setWrapMode(Texture::WrapMode::REPEAT);
	m_assetmanager.getTexture("res/images/textures/wall.png")->bind();
}
