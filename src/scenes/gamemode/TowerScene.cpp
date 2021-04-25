#include "scenes/gamemode/TowerScene.hpp"

extern "C" {
	void ffi_TowerScene_spawnPlayer(Engine *engine, glm::vec3 pos, glm::vec3 vel, float sx, float sy) {
		entt::registry &m_registry = ((TowerScene *)engine->getScene())->m_registry;
		AssetManager &m_assetmanager = ((TowerScene *)engine->getScene())->m_assetmanager;

		auto entity = m_registry.create();
		m_registry.emplace<CPosition>(entity, pos);
		m_registry.emplace<CVelocity>(entity, vel);
		m_registry.emplace<CBillboard>(entity,
			m_assetmanager.getTexture("res/images/textures/dungeon.png"), glm::vec2(0.2f), glm::vec3(0.f));
		m_registry.get<CBillboard>(entity).setSubRect(sx * 16.0f, sy * 16.0f, 16.0f, 16.0f, 256, 256);
		m_registry.emplace<CGravity>(entity);
		m_registry.emplace<CTerrainCollider>(entity, false);
	}

	void ffi_TowerScene_toMainMenu(Engine *engine) {
		engine->setActiveScene(new MainMenuScene());
	}
}

bool TowerScene::onCreate() {
	m_registry.set<entt::dispatcher>();

	m_camera = std::make_shared<Camera>(glm::vec3(5.f, 5.f, 5.f));
	m_camera->setTarget(glm::vec3(0.f));

	ffi_TowerScene_spawnPlayer(m_engine, glm::vec3(-1.f, .5f, 0.f), glm::vec3(0.f), 8.f, 14.f);
	ffi_TowerScene_spawnPlayer(m_engine, glm::vec3( 1.f, .5f, 0.f), glm::vec3(0.f), 7.f, 14.f);
	loadSystems();
	loadTerrainShader();

	return true;
}

void TowerScene::onDestroy() {

}

void TowerScene::onUpdate(float dt) {
	std::for_each(m_updatesystems.begin(), m_updatesystems.end(), [dt](auto &usys) { usys->update(dt); });

}

void TowerScene::onRender() {
	std::for_each(m_rendersystems.begin(), m_rendersystems.end(), [](auto &rsys) { rsys->draw(); });

	imguiLuaJitConsole(m_engine->getLuaState());

	updateTerrainShader();
	m_terrain.draw(&m_chunkshader);

}

/////////////
// PRIVATE //
/////////////

void TowerScene::loadSystems() {
	// register a logging event system
	m_logsystem = std::make_unique<LogSystem>(m_registry);
	m_logsystem->setLogger([](const LogEvent &e) {
		std::cout << "TOWERS: " << e.text << std::endl;
	});
	
	// initialize update and render systems
	auto billboardRenderSystem = std::make_shared<BillboardRenderSystem>(m_registry, m_camera);
	auto textRenderSystem = std::make_shared<TextEventSystem>(m_registry, m_camera);
	auto wayfindSystem = std::make_shared<WayfindSystem>(m_registry, m_camera);
	auto primitiveRenderer = std::make_shared<PrimitiveRenderSystem>(m_registry, m_camera);
	
	// create update systems
	//m_updatesystems.emplace_back(new TerrainCollisionSystem(m_registry, chunkedWorld));
	m_updatesystems.emplace_back(new CharacterControllerSystem(m_registry, m_engine->getWindow(), &m_camera));
	m_updatesystems.emplace_back(new GravitySystem(m_registry, 0.000981f));
	m_updatesystems.emplace_back(new RandomJumpSystem(m_registry, 0.003f));
	m_updatesystems.push_back(wayfindSystem);
	m_updatesystems.emplace_back(new PressAwaySystem(m_registry));
	m_updatesystems.emplace_back(new PositionUpdateSystem(m_registry));
	m_updatesystems.push_back(billboardRenderSystem);
	m_updatesystems.push_back(textRenderSystem);
	m_updatesystems.push_back(primitiveRenderer);
	m_updatesystems.emplace_back(new DespawnSystem(m_registry));
	m_updatesystems.emplace_back(new AudioSystem(m_registry, m_assetmanager));
	
	// and render systems
	//m_rendersystems.emplace_back(new TerrainRenderSystem(m_registry, m_camera, assetManager, chunkedWorld));
	m_rendersystems.emplace_back(new DecalRenderSystem(m_registry, m_camera));
	m_rendersystems.push_back(billboardRenderSystem);
	m_rendersystems.push_back(textRenderSystem);
	m_rendersystems.push_back(wayfindSystem);
	m_rendersystems.push_back(primitiveRenderer);
}

void TowerScene::loadTerrainShader() {
	m_chunkshader.load("res/glsl/proto/terrain_vert.glsl", sgl::shader::VERTEX);
	m_chunkshader.load("res/glsl/proto/terrain_frag.glsl", sgl::shader::FRAGMENT);
	m_chunkshader.compile();
	m_chunkshader.link();
}

void TowerScene::updateTerrainShader() {
	m_chunkshader["uProj"] = m_camera->getProjection();
	m_chunkshader["uView"] = m_camera->getView();
	m_chunkshader["uModel"] = glm::mat4(1.f);
	m_chunkshader["uTextureTopdownScale"] = 2.0f;
	m_chunkshader["uTextureSideScale"] = 2.0f;
	m_chunkshader["uTextureTopdown"] = 0;
	m_chunkshader["uTextureSide"] = 1;
	m_chunkshader["uTime"] = (float)glfwGetTime();

	glActiveTexture(GL_TEXTURE0);
	m_assetmanager.getTexture("res/images/textures/floor.png")->setWrapMode(Texture::WrapMode::REPEAT);
	m_assetmanager.getTexture("res/images/textures/floor.png")->bind();
	
	glActiveTexture(GL_TEXTURE1);
	m_assetmanager.getTexture("res/images/textures/wall.png")->setWrapMode(Texture::WrapMode::REPEAT);
	m_assetmanager.getTexture("res/images/textures/wall.png")->bind();
}
