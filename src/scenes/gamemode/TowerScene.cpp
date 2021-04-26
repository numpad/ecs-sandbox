#include "scenes/gamemode/TowerScene.hpp"

extern "C" {
	entt::entity ffi_TowerScene_spawnDefaultEntity(Engine *engine, glm::vec3 pos, glm::vec3 vel, float sx, float sy) {
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
		return entity;
	}
	entt::entity ffi_TowerScene_spawnBomb(Engine *engine, glm::vec3 pos, glm::vec3 vel) {
		entt::registry &m_registry = ((TowerScene *)engine->getScene())->m_registry;
		AssetManager &m_assetmanager = ((TowerScene *)engine->getScene())->m_assetmanager;

		auto entity = ffi_TowerScene_spawnDefaultEntity(engine, pos, vel, 15.f, 3.f);
		return entity;
	}

	void ffi_TowerScene_toMainMenu(Engine *engine) {
		engine->setActiveScene(new MainMenuScene());
	}

	void ffi_TowerScene_addSphere(Engine *engine, glm::vec3 p, float r) {
		((TowerScene *)engine->getScene())->m_terrain.add_body(new SphereBody(p, r));
	}

	void ffi_TowerScene_subSphere(Engine *engine, glm::vec3 p, float r) {
		((TowerScene *)engine->getScene())->m_terrain.sub_body(new SphereBody(p, r));
	}

	// tmp shortcuts:
	void srem(Engine *engine, glm::vec3 p, float r, float h) {
		((TowerScene *)engine->getScene())->m_terrain.sub_body(new DiskBody(p, r, h));
	}
}

bool TowerScene::onCreate() {
	m_registry.set<entt::dispatcher>();

	m_camera = std::make_shared<Camera>(glm::vec3(5.f, 5.f, 5.f));
	m_camera->setTarget(glm::vec3(0.f));

	m_players.push_back(ffi_TowerScene_spawnDefaultEntity(m_engine, glm::vec3(-1.f, .5f, 0.f), glm::vec3(0.f), 8.f, 14.f));
	m_players.push_back(ffi_TowerScene_spawnDefaultEntity(m_engine, glm::vec3( 1.f, .5f, 0.f), glm::vec3(0.f), 7.f, 14.f));
	loadSystems();
	loadTerrainShader();

	return true;
}

void TowerScene::onDestroy() {

}

void TowerScene::onUpdate(float dt) {
	// testing: rotate camera around origin
	static float angle = 0.f;
	static float basedist = 8.f;
	float dist = basedist + glm::sin(angle * 3.8f + 3.f) * 0.4f;
	angle += 0.0092f;
	m_camera->setPos(glm::vec3(glm::cos(angle) * dist, 5.f, glm::sin(angle) * dist));
	
	// testing: spawn bombs, replace with lua script later on
	static Random bomb_random(-1.f, 1.f);
	static float dt_sum = 0.f;
	constexpr float dt_max = 2.7f;
	dt_sum += dt;
	while (dt_sum >= dt_max) {
		dt_sum -= dt_max;
		entt::entity bomb = ffi_TowerScene_spawnBomb(m_engine, glm::vec3(bomb_random() * 2.f, .5f, bomb_random() * 2.f), glm::vec3(bomb_random() * 0.1f, bomb_random() * 0.04f, bomb_random() * 0.1f));
	}

	std::for_each(m_updatesystems.begin(), m_updatesystems.end(), [dt](auto &usys) { usys->update(dt); });

}

void TowerScene::onRender() {
	updateTerrainShader();
	m_terrain.draw(&m_chunkshader);
	
	std::for_each(m_rendersystems.begin(), m_rendersystems.end(), [](auto &rsys) { rsys->draw(); });

	imguiLuaJitConsole(m_engine->getLuaState());

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
	m_updatesystems.emplace_back(new DistanceFunctionCollisionSystem(m_registry, m_terrain));
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
