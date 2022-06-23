#include "scenes/gamemode/TowerScene.hpp"

extern "C" {
entt::entity ffi_TowerScene_spawnDefaultEntity(Engine* engine, glm::vec3 pos, glm::vec3 vel, float sx, float sy) {
	entt::registry& m_registry = ((TowerScene*)engine->getScene())->m_registry;
	AssetManager& m_assetmanager = ((TowerScene*)engine->getScene())->m_assetmanager;

	entt::entity entity = m_registry.create();

	Texture* texture = m_assetmanager.getTexture("res/images/sprites/people_oriented.png");
	m_registry.emplace<CPosition>(entity, pos);
	m_registry.emplace<CVelocity>(entity, vel);
	m_registry.emplace<CBillboard>(entity, texture, glm::vec2(0.2f));
	m_registry.emplace<CTextureRegion>(entity, sx * 16.0f, sy * 16.0f, 16.0f, 16.0f, 128, 128);
	m_registry.emplace<CGravity>(entity);
	m_registry.emplace<CTerrainCollider>(entity, false);
	return entity;
}
entt::entity ffi_TowerScene_spawnBomb(Engine* engine, glm::vec3 pos, glm::vec3 vel) {
	entt::registry& m_registry = ((TowerScene*)engine->getScene())->m_registry;
	AssetManager& m_assetmanager = ((TowerScene*)engine->getScene())->m_assetmanager;

	static Random random_size(0.25f, 0.45f);
	Texture* texture = m_assetmanager.getTexture("res/images/textures/dungeon.png");

	entt::entity entity = m_registry.create();
	m_registry.emplace<CPosition>(entity, pos);
	m_registry.emplace<CVelocity>(entity, vel);
	m_registry.emplace<CBillboard>(entity, texture, glm::vec2(0.2f));
	m_registry.emplace<CTextureRegion>(entity, 15.f * 16.0f, 3.f * 16.0f, 16.0f, 16.0f, 256, 256);
	m_registry.emplace<CGravity>(entity);
	m_registry.emplace<CTerrainCollider>(entity, false);
	m_registry.emplace<CHealth>(entity, 1);
	m_registry.emplace<CDamageOverTime>(entity, 1, 2.f, 99999.f);
	m_registry.emplace<CExplosive>(entity, random_size());

	return entity;
}

void ffi_TowerScene_spawnExplosionParticles(Engine* engine, glm::vec3 pos) {
	entt::registry& m_registry = ((TowerScene*)engine->getScene())->m_registry;
	AssetManager& m_assetmanager = ((TowerScene*)engine->getScene())->m_assetmanager;

	Texture* texture = m_assetmanager.getTiledTexture("res/images/particles/smoke.png", 8, 8, 0, 0);

	constexpr float TWO_PI = glm::pi<float>() * 2.f;
	static Random random_range(0.f, 1.f);
	// ring
	const float ring_particles = 24.f * engine->getConfig().particle_amount;
	for (float angle = 0.f; angle < TWO_PI; angle += TWO_PI / ring_particles) {
		const glm::vec2 dir = m3d::angleToDirection(angle);

		entt::entity entity = m_registry.create();
		m_registry.emplace<CPosition>(entity, pos);
		m_registry.emplace<CVelocity>(entity, glm::vec3(dir.x, 0.f, dir.y));
		m_registry.emplace<CBillboard>(entity, texture, glm::vec2(0.2f));
		m_registry.emplace<CTextureRegion>(entity, 0.f * 16.0f, 0.f * 16.0f, 16.0f, 16.0f, 128, 128);
		m_registry.emplace<CHealth>(entity, 1);
		m_registry.emplace<CDamageOverTime>(entity, 1, 0.5f);
	}
	// center
	const float center_particles = 50.f * engine->getConfig().particle_amount;
	for (float angle = 0.f; angle < TWO_PI; angle += TWO_PI / center_particles) {
		glm::vec3 dir = glm::normalize(m3d::randomizeVec3(glm::vec3(0.f), 1.f));
		dir.y = glm::abs(dir.y);

		entt::entity entity = m_registry.create();
		m_registry.emplace<CPosition>(entity, pos);
		m_registry.emplace<CVelocity>(entity, glm::vec3(dir.x, dir.y, dir.z) * random_range() * 0.08f);
		m_registry.emplace<CBillboard>(entity, texture, glm::vec2(0.2f));
		m_registry.emplace<CTextureRegion>(entity, 0.f * 16.0f, 0.f * 16.0f, 16.0f, 16.0f, 128, 128);
		m_registry.emplace<CHealth>(entity, 1);
		m_registry.emplace<CDamageOverTime>(entity, 1, 0.5f);
	}
	// debris
	const float debris_particles = 4.f * engine->getConfig().particle_amount;
	for (float angle = 0.f; angle < TWO_PI; angle += TWO_PI / debris_particles) {
		glm::vec3 dir = glm::normalize(m3d::randomizeVec3(glm::vec3(0.f), 1.f));
		dir.y = glm::abs(dir.y);

		entt::entity entity = m_registry.create();
		m_registry.emplace<CPosition>(entity, pos);
		m_registry.emplace<CVelocity>(entity, glm::vec3(dir.x, dir.y, dir.z) * random_range() * 0.08f);
		m_registry.emplace<CBillboard>(entity, texture, glm::vec2(0.2f));
		m_registry.emplace<CTextureRegion>(entity, 0.f * 16.0f, 0.f * 16.0f, 16.0f, 16.0f, 128, 128);
		m_registry.emplace<CGravity>(entity);
		m_registry.emplace<CHealth>(entity, 1);
		m_registry.emplace<CDamageOverTime>(entity, 1, 2.5f);
	}
}

entt::entity ffi_TowerScene_spawnDecal(Engine* engine, glm::vec3 pos) {
	entt::registry& m_registry = ((TowerScene*)engine->getScene())->m_registry;
	AssetManager& m_assetmanager = ((TowerScene*)engine->getScene())->m_assetmanager;

	Texture* texture = m_assetmanager.getTexture("res/images/decals/coords.png");
	entt::entity entity = m_registry.create();
	m_registry.emplace<CPosition>(entity, pos);
	m_registry.emplace<CDecal>(entity, glm::vec3(0.2f, 0.05f, 0.2f), texture, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
	m_registry.emplace<COrientation>(entity);

	return entity;
}

void ffi_TowerScene_toMainMenu(Engine* engine) {
	engine->setActiveScene(new MainMenuScene());
}

void ffi_TowerScene_addSphere(Engine* engine, glm::vec3 p, float r) {
	((TowerScene*)engine->getScene())->m_terrain.add_body(new SphereBody(p, r));
}

void ffi_TowerScene_subSphere(Engine* engine, glm::vec3 p, float r) {
	((TowerScene*)engine->getScene())->m_terrain.sub_body(new SphereBody(p, r));
}

// tmp shortcuts:
void ffi_TowerScene_subDisk(Engine* engine, glm::vec3 p, float r, float h) {
	((TowerScene*)engine->getScene())->m_terrain.sub_body(new DiskBody(p, r, h));
}
}

bool TowerScene::onCreate() {
	m_registry.set<entt::dispatcher>();

	m_registry.ctx<entt::dispatcher>().sink<KillEntityEvent>().connect<&TowerScene::onEntityKilled>(this);
	m_registry.ctx<entt::dispatcher>().sink<ExplosionEvent>().connect<&TowerScene::onBombExplodes>(this);
	m_engine->getDispatcher().sink<MouseButtonEvent>().connect<&TowerScene::onMouseButtonInput>(this);

	m_camera = std::make_shared<Camera>(glm::vec3(5.f, 5.f, 5.f));
	m_camera->setTarget(glm::vec3(0.f));

	m_players.push_back(
	    ffi_TowerScene_spawnDefaultEntity(m_engine, glm::vec3(-1.f, .5f, 0.f), glm::vec3(0.f), 0.f, 0.f));
	m_players.push_back(
	    ffi_TowerScene_spawnDefaultEntity(m_engine, glm::vec3(1.f, .5f, 0.f), glm::vec3(0.f), 0.f, 1.f));
	m_registry.emplace<CKeyboardControllable>(m_players[0], 0.0007f, GLFW_KEY_W, GLFW_KEY_S, GLFW_KEY_A, GLFW_KEY_D,
	                                          GLFW_KEY_X);
	m_registry.emplace<CKeyboardControllable>(m_players[1], 0.0007f, GLFW_KEY_I, GLFW_KEY_K, GLFW_KEY_J, GLFW_KEY_L,
	                                          GLFW_KEY_M);

	m_registry.emplace<COrientedTexture>(m_players[0], 8, 0.f);
	m_registry.emplace<COrientedTexture>(m_players[1], 8, 0.f);

	constexpr size_t nlights = 3;
	const entt::entity lights[nlights] = {m_registry.create(), m_registry.create(), m_registry.create()};
	for (size_t i = 0; i < nlights; ++i) {
		const glm::vec2 pos = m3d::angleToDirection(glm::radians(i * 360.0f / (float)nlights)) * 0.66f;
		m_registry.emplace<CPosition>(lights[i], glm::vec3(pos.x, 0.0f, pos.y));
		m_registry.emplace<CPointLight>(lights[i], 1.0f, glm::vec3(1.0f * (i == 0), 1.0f * (i == 1), 1.0f * (i == 2)));
	}

	loadSystems();
	loadTerrainShader();

	m_terminal.update();

	lua_State* L = m_engine->getLuaState();
	if (luaL_dofile(L, "res/scripts/tower_scene.lua")) {
		const char* err = lua_tostring(L, -1);
		fmt::print(fmt::fg(fmt::terminal_color::red), "{}\n", err);
		lua_pop(L, 1);
		return false;
	}

	lua_State* T = lua_newthread(L);
	lua_getglobal(T, "update");
	lua_resume(T, 0);
	lua_resume(T, 0);

	return true;
}

void TowerScene::onDestroy() {
	m_registry.ctx<entt::dispatcher>().sink<KillEntityEvent>().disconnect(this);
	m_registry.ctx<entt::dispatcher>().sink<ExplosionEvent>().disconnect(this);
	m_engine->getDispatcher().sink<MouseButtonEvent>().disconnect(this);
}

void TowerScene::onUpdate(float dt) {
	// testing: rotate camera around origin
	static float angle = 0.f;
	static float basedist = 6.5f;
	static float rotation_speed = 0.0024f;
	static float height = 3.5f;
	float dist = basedist + glm::sin(angle * 3.8f + 3.f) * 0.15f;
	angle += rotation_speed;
	m_camera->setPos(glm::vec3(glm::cos(angle) * dist, height, glm::sin(angle) * dist));

	static float timescale = 1.f;

	static glm::vec3 crosspos = glm::vec3(0.0f);
	imguiRenderMenuBar(m_engine, m_registry, crosspos, dt);
	if (ImGui::Begin("Towers")) {
		ImGui::Text("Camera");
		ImGui::Text("Registry size: %d", m_registry.size());
		ImGui::SliderFloat("Distance", &basedist, 1.f, 10.f);
		ImGui::DragFloat("Rotation", &rotation_speed, 0.0002f);
		ImGui::SliderFloat("Height", &height, 0.f, 7.f);
		ImGui::Text("Settings");
		ImGui::SliderFloat("Time scale [WIP]", &timescale, 0.f, 2.f);
	}
	ImGui::End();

	if (ImGui::Begin("Terminal")) {
		ImGui::Image((void*)m_terminal.getTexture(), ImVec2(256.0f, 256.0f), ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
	}
	ImGui::End();

	// testing: spawn bombs, replace with lua script later on
	static Random bomb_random(-1.f, 1.f);
	static float dt_sum = 0.f;
	static float dt_max = 4.3f;
	dt_sum += dt;
	if (dt_sum >= dt_max) {
		dt_sum -= dt_max;
		// spawn bomb at player pos so they dont camp, randomize time until next bomb a bit
		dt_max = 3.f + bomb_random() * 1.4f;
		// ffi_TowerScene_spawnBomb(m_engine, m_registry.get<CPosition>(m_players[0]).pos + glm::vec3(0.f, .5f, 0.f) +
		// glm::vec3(0.f, .5f, 0.f), glm::vec3(0.f)); ffi_TowerScene_spawnBomb(m_engine,
		// m_registry.get<CPosition>(m_players[1]).pos + glm::vec3(0.f, .5f, 0.f), glm::vec3(0.f));
	}

	m_registry.ctx<entt::dispatcher>().update();
	std::for_each(m_updatesystems.begin(), m_updatesystems.end(), [dt](auto& usys) { usys->update(dt * timescale); });
}

void TowerScene::onRender() {
	GLState terrainState;
	terrainState.depth_write = true;
	terrainState.depth_test = true;
	terrainState.cull_face = true;
	Engine::Instance->getGraphics().setState(terrainState);

	updateTerrainShader();
	m_terrain.draw(&m_chunkshader);

	std::for_each(m_rendersystems.begin(), m_rendersystems.end(), [](auto& rsys) { rsys->draw(); });

	imguiLuaJitConsole(m_engine->getLuaState());

	imguiGamepadInfo();
}

/////////////
// PRIVATE //
/////////////

void TowerScene::loadSystems() {
	// register a logging event system
	m_logsystem = std::make_unique<LogSystem>(m_registry);
	m_logsystem->setLogger([](const LogEvent& e) { std::cout << "TOWERS: " << e.text << std::endl; });

	// initialize update and render systems
	auto billboardRenderSystem = std::make_shared<BillboardRenderSystem>(m_registry, m_camera);

	// create update systems
	m_updatesystems.emplace_back(new DistanceFunctionCollisionSystem(m_registry, m_terrain));
	m_updatesystems.emplace_back(new CharacterControllerSystem(m_registry, m_engine->getWindow(), &m_camera));
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
	m_rendersystems.emplace_back(new LightVolumeRenderSystem(m_registry, m_camera));
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
	m_chunkshader["uModel"] = glm::mat4(1.0f);
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

void TowerScene::onEntityKilled(const KillEntityEvent& event) {
	auto found = std::find(m_players.begin(), m_players.end(), event.which);
	bool is_player = (found != m_players.end());

	if (is_player) {
		// play sound
		static Random random(0.8f, 1.2f);
		m_registry.ctx<entt::dispatcher>().enqueue<PlaySoundEvent>("res/audio/sfx/ouch.wav", random());

		// remove player
		m_players.erase(found);

		// switch to main menu once a player won
		if (m_players.size() == 1) {
			m_engine->setActiveScene(new MainMenuScene());
		}
	}
}

void TowerScene::onBombExplodes(const ExplosionEvent& event) {
	// remove terrain
	ffi_TowerScene_subDisk(m_engine, event.position, event.radius, 1.0f);

	// play sound
	m_registry.ctx<entt::dispatcher>().enqueue<PlaySoundEvent>("res/audio/sfx/explode.wav",
	                                                           1.3f - event.radius * 0.92f);
	ffi_TowerScene_spawnExplosionParticles(m_engine, event.position);
}

void TowerScene::onMouseButtonInput(const MouseButtonEvent& event) {
	if (!event.is_down)
		return;

	glm::vec2 mpos = m_engine->getWindow().getNormalizedMousePosition();
	m3d::ray ray = m_camera->raycast(mpos);
	float dist = m3d::raycast(m_terrain, ray.origin, ray.dir, 90.f);

	if (dist > 0.f) {
		const glm::vec3 pos = ray.origin + ray.dir * dist;
		switch (event.button) {
		case GLFW_MOUSE_BUTTON_2: {
			const float angleOffset = Random(0.0f, glm::pi<float>() * 2.0f)();
			for (float angle = 0.0f; angle <= glm::pi<float>() * 2.0f; angle += glm::pi<float>() * 0.5f) {
				ffi_TowerScene_spawnBomb(
				    m_engine, pos + glm::vec3(0.0f, 0.01f, 0.0f),
				    glm::vec3(glm::cos(angle + angleOffset) * -0.1f, 0.01f, glm::sin(angle + angleOffset) * -0.1f));
			}
		}
		case GLFW_MOUSE_BUTTON_1:
			ffi_TowerScene_spawnBomb(m_engine, pos, glm::vec3(0.f));
			ffi_TowerScene_spawnDecal(m_engine, pos);
			break;
		}
	}
}
