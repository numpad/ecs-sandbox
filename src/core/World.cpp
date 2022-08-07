#include <World.hpp>
#include <Engine/Engine.hpp>

using namespace glm;

World::World(Engine* engine, std::shared_ptr<Camera> tcamera) : camera(tcamera), m_engine(engine) {
	registry.set<entt::dispatcher>();
	chunkedWorld = std::make_shared<ChunkedWorld>(vec3(2.f, 1.f, 2.f));

	// initialize lua
	setupLua();

	// load systems
	loadSystems();
}

World::~World() {
	registry.ctx<entt::dispatcher>().trigger<LogEvent>("rip world");
}

void World::destroy() {
	updateSystems.clear();
	renderSystems.clear();
	destroyFloor();
	registry.clear();
}

void World::setupLua() {
	lua_State* L = m_engine->getLuaState();

	lua_pushlightuserdata(L, this);
	lua_setglobal(L, "_World");
	lua_pushlightuserdata(L, &tileGrid);
	lua_setglobal(L, "_tileGrid");
	lua_pushlightuserdata(L, chunkedWorld.get());
	lua_setglobal(L, "_chunkedWorld");
	lua_pushlightuserdata(L, &chunkedWorld->getTerrain());
	lua_setglobal(L, "_chunkedTerrain");
}

entt::entity World::getNearestEntity(vec3 posNear) {
	entt::entity nearest = entt::null;

	registry.view<CPosition>().each([this, &nearest, posNear](auto entity, auto& pos) {
		if (entity != this->worldCrosshair) {
			if (nearest == entt::null) {
				nearest = entity;
				return;
			}

			if (distance(posNear, pos.pos) < distance(posNear, this->registry.get<CPosition>(nearest).pos)) {
				nearest = entity;
			}
		}
	});

	return nearest;
}

entt::entity World::spawnDefaultEntity(vec3 pos) {
	static Random rand;
	vec2 rsize(0.2f, 0.2f);

	Texture* texture = this->assetManager.getTexture("res/images/textures/dungeon.png");

	auto entity = registry.create();
	registry.emplace<CPosition>(entity, pos);
	registry.emplace<CVelocity>(entity, vec3(0.f));
	registry.emplace<CBillboard>(entity, texture, rsize);
	registry.emplace<CTextureRegion>(entity, 1.0f * 16.0f, 10.0f * 16.0f, 16.0f, 16.0f, 256, 256);
	registry.emplace<CGravity>(entity);
	registry.emplace<CSphereCollider>(entity, 0.045f, 0.01f);
	registry.emplace<CJumpTimer>(entity, 0);
	registry.emplace<CHealth>(entity, 10);
	registry.emplace<CTerrainCollider>(entity, false);
	if (registry.valid(this->player)) {
		// registry.emplace<CRunningToTarget>(entity, this->player, 0.001f, 0.2f);
	}
	return entity;
}

void World::resetEntities() {
	vec3 spawnPos;
	if (registry.try_get<CPosition>(getPlayer()))
		spawnPos = registry.get<CPosition>(getPlayer()).pos;

	registry.clear();
	spawnPlayer(spawnPos);
}

entt::entity World::spawnPlayer(vec3 pos) {
	if (registry.valid(player))
		registry.destroy(this->player);

	this->player = spawnDefaultEntity(pos);
	registry.emplace<CKeyboardControllable>(this->player, 0.003f);
	TiledTexture* playertex = assetManager.getTiledTexture("res/images/sprites/guy_stand_frames.png", 16, 16, 0, 0);
	registry.emplace_or_replace<CBillboard>(this->player, playertex, vec2(0.2f, 0.2f));
	registry.emplace_or_replace<CTextureRegion>(this->player, 0.0f * 16.0f, 0.0f * 16.0f, 16.0f, 16.0f, 96, 16);

	registry.remove<CJumpTimer>(this->player);

	registry.emplace_or_replace<CSpawnPoint>(this->player, vec3(0.0f, 0.5f, 0.0f));
	registry.emplace_or_replace<CHealth>(this->player, 12);

	registry.emplace_or_replace<CTerrainCollider>(this->player, false);

	// world pos crosshair
	worldCrosshair = registry.create();
	registry.emplace<CPosition>(worldCrosshair, vec3(0.0f));
	// TODO: assetManager.newTiledTexture("res/...", tile_x, tile_y, tileset_width, tileset_height)
	// registry.emplace<CBillboard>(worldCrosshair, assetManager.getTexture("res/images/sprites/arrows.png"),
	//	vec2(0.2f, 0.2f), vec3(0.0f, 1.0f, 0.0f));
	TiledTexture* tiledtex = assetManager.getTiledTexture("res/images/sprites/arrows.png", 64, 64, 0, 0);
	registry.emplace<CBillboard>(worldCrosshair, tiledtex, vec2(0.4f, 0.4f));
	registry.emplace<CTextureRegion>(worldCrosshair, tiledtex->getSubRect());

	return this->player;
}

void World::load() {
	// load world
	setupFloor();

	// spawn player
	spawnPlayer();

	loaded = true;
}

void World::update() {
#ifndef NDEBUG
	Benchmark update_benchmark;
	static std::unordered_map<const char*, std::vector<double>> exec_time;
#endif

	for (auto& sys : updateSystems) {
#ifndef NDEBUG
		update_benchmark.start();
#endif

		sys->update(1.f / 60.f);

#ifndef NDEBUG
		update_benchmark.stop();
		if (exec_time.find(typeid(*sys).name()) == exec_time.end()) {
			exec_time[typeid(*sys).name()] = std::vector<double>();
		}
		exec_time[typeid(*sys).name()].push_back(update_benchmark.ms());
#endif
	}

#ifndef NDEBUG
	if (ImGui::Begin("Systems")) {
		using namespace ImGui;
		static int vals = 60;
		InputInt("last x values", &vals, 1);

		Columns(4);
		Text("SYSTEM");
		NextColumn();
		Text("t");
		NextColumn();
		Text("avg");
		NextColumn();
		Text("min/max");
		NextColumn();

		for (auto& it : exec_time) {
			Text("%s", it.first);
			NextColumn();
			Text("%.3fms", it.second.back());
			NextColumn();
			double total = 0.0;
			double min = std::numeric_limits<double>::max();
			double max = std::numeric_limits<double>::min();
			for (auto it2 = it.second.end() - vals; it2 != it.second.end(); ++it2) {
				total += *it2;
				min = std::min(min, *it2);
				max = std::max(max, *it2);
			}
			Text("%.3fms", total / double(vals));
			NextColumn();
			Text("%.3fms/%.3fms", min, max);
			NextColumn();
		}
	}
	ImGui::End();
#endif

#ifndef NDEBUG
	if (!registry.valid(player) && this->is_loaded()) {
		spawnPlayer();
		registry.ctx<entt::dispatcher>().trigger<LogEvent>("World: respawned player because debug is enabled.",
		                                                   LogEvent::WARN);
	}
#endif
}

void World::draw() {
	// render systems

	// TODO: remove this, just for testing
	if (ImGui::Begin("Edit world")) {
		static int vtype = 0;
		constexpr int vsphere = 0, vcube = 1;
		ImGui::RadioButton("Sphere", &vtype, vsphere);
		ImGui::SameLine();
		ImGui::RadioButton("Cube", &vtype, vcube);

		static int vop = (int)SignedDistTerrain::Op::UNION;
		ImGui::RadioButton("Add", &vop, (int)SignedDistTerrain::Op::UNION);
		ImGui::SameLine();
		ImGui::RadioButton("Sub", &vop, (int)SignedDistTerrain::Op::DIFF);

		static vec3 pos;
		static vec3 r = vec3(.5f);
		ImGui::InputFloat3("pos", &pos[0], "%.2f");
		if (vtype == vsphere) {
			ImGui::SliderFloat("r", &r[0], .1f, 2.f);
		} else {
			ImGui::SliderFloat3("r", &r[0], .1f, 2.f);
		}

		if (ImGui::Button("Edit")) {
			ivec2 chunkPos = chunkedWorld->getTerrain().worldPosToChunk(pos);
			SignedDistTerrain* e = (SignedDistTerrain*)chunkedWorld->getTerrain().getChunkGrid().at(chunkPos);
			if (!e) {
				e = new SignedDistTerrain();
			}

			vec3 localPos = chunkedWorld->getTerrain().worldPosToLocalChunkPos(pos);
			if (vtype == vsphere) {
				e->sphere(localPos, r.x, (SignedDistTerrain::Op)vop);
			} else if (vtype == vcube) {
				e->box(localPos, r, (SignedDistTerrain::Op)vop);
			}
			chunkedWorld->update(chunkPos, e);
		}
	}
	ImGui::End();

	// BM_START(rendering, 30);
	for (auto& sys : renderSystems)
		sys->draw();
	// BM_STOP(rendering);
}

void World::setCamera(std::shared_ptr<Camera> camera) {
	this->camera = camera;
	for (auto& sys : renderSystems)
		sys->setCamera(camera);
}

///////////////
/// PRIVATE ///
///////////////

void World::loadSystems() {
	// clear all
	updateSystems.clear();

	// register a logging event system
	logEventSystem = std::make_unique<LogSystem>(registry);
	logEventSystem->setLogger([](const LogEvent& e) {
		std::cout << e.text << std::endl;
	});

	// initialize update and render systems
	auto billboardRenderSystem = std::make_shared<BillboardRenderSystem>(registry, camera);
	auto textRenderSystem = std::make_shared<TextEventSystem>(registry, camera);
	auto wayfindSystem = std::make_shared<WayfindSystem>(registry, camera);
	auto primitiveRenderer = std::make_shared<PrimitiveRenderSystem>(registry, camera);

	// create update systems
	updateSystems.emplace_back(new TerrainCollisionSystem(registry, chunkedWorld));
	updateSystems.emplace_back(new CharacterControllerSystem(registry, m_engine->window, &camera));
	updateSystems.emplace_back(new GravitySystem(registry, 0.000981f));
	updateSystems.emplace_back(new RandomJumpSystem(registry, 0.003f));
	updateSystems.push_back(wayfindSystem);
	updateSystems.emplace_back(new PressAwaySystem(registry));
	updateSystems.emplace_back(new PositionUpdateSystem(registry));
	updateSystems.push_back(billboardRenderSystem);
	updateSystems.push_back(textRenderSystem);
	updateSystems.push_back(primitiveRenderer);
	updateSystems.emplace_back(new DespawnSystem(registry));
	updateSystems.emplace_back(new AudioSystem(registry, assetManager));

	// and render systems
	renderSystems.emplace_back(new TerrainRenderSystem(registry, camera, assetManager, chunkedWorld));
	renderSystems.emplace_back(new DecalRenderSystem(registry, camera));
	renderSystems.push_back(billboardRenderSystem);
	renderSystems.push_back(textRenderSystem);
	renderSystems.push_back(wayfindSystem);
	renderSystems.push_back(primitiveRenderer);
}

void World::setupFloor() {
	if (luaL_dofile(m_engine->getLuaState(), "res/scripts/world/mapgen.lua") != 0) {
		std::cerr << "[LUA] Error: " << lua_tostring(m_engine->getLuaState(), -1) << std::endl;
		lua_pop(m_engine->getLuaState(), 1);
	}

	spawnDefaultEntity(vec3(.5f, 1.f, .5f));

	double starttime = glfwGetTime();
	size_t i = 0;
	chunkedWorld->getTerrain().getChunkGrid().each([this, &i](int x, int y, Terrain* terrain) {
		registry.ctx<entt::dispatcher>().trigger<LogEvent>("World: Generating chunk " + std::to_string(i++),
		                                                   LogEvent::LOG);
		this->chunkedWorld->polygonizeChunk(ivec2(x, y));
	});
#ifndef NDEBUG
	double endtime = glfwGetTime();
	registry.ctx<entt::dispatcher>().trigger<LogEvent>(
	    "World: Generated chunks in " + std::to_string(endtime - starttime) + "s.", LogEvent::LOG);
#endif
}

void World::destroyFloor() {
	chunkedWorld->destroy();
}

lua_State* World::getLuaState() const {
	return m_engine->getLuaState();
}
