#include <World.hpp>

using namespace glm;

World::World(GLFWwindow *window, std::shared_ptr<Camera> camera)
	: camera(camera),
	chunkedWorld(vec3(2.f)),
	charControllerSystem(window)
{
	registry.set<entt::dispatcher>();
	
	// load systems
	loadSystems();
	
	// load terrain shader
	chunkShader.load("res/glsl/proto/terrain_vert.glsl", sgl::shader::VERTEX);
	chunkShader.load("res/glsl/proto/terrain_frag.glsl", sgl::shader::FRAGMENT);
	chunkShader.compile();
	chunkShader.link();
	Blackboard::write("chunkShader", &chunkShader);
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

entt::entity World::getNearestEntity(vec3 posNear) {
	entt::entity nearest = entt::null;
	
	registry.view<CPosition>().each([this, &nearest, posNear](auto entity, auto &pos) {
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
	
	vec3 rcol(rand() * 0.5f + 0.5f, rand() * 0.5f + 0.5f, rand() * 0.5f + 0.5f);
	
	auto entity = registry.create();
	registry.emplace<CPosition>(entity, pos);
	registry.emplace<CVelocity>(entity, vec3(0.f));
	registry.emplace<CBillboard>(entity,
		this->assetManager.getTexture("res/images/textures/dungeon.png"), rsize, rcol);
	registry.get<CBillboard>(entity).setSubRect(1.0f * 16.0f, 10.0f * 16.0f, 16.0f, 16.0f, 256, 256);
	registry.emplace<CGravity>(entity);
	registry.emplace<CSphereCollider>(entity, 0.045f, 0.01f);
	registry.emplace<CJumpTimer>(entity, 0);
	registry.emplace<CHealth>(entity, 10);
	if (registry.valid(this->player)) {
		//registry.emplace<CRunningToTarget>(entity, this->player, 0.001f, 0.2f);
	}
	return entity;
}

void World::resetEntities() {
	vec3 spawnPos;
	if (registry.has<CPosition>(getPlayer()))
		spawnPos = registry.get<CPosition>(getPlayer()).pos;
	
	registry.clear();
	spawnPlayer(spawnPos);
}

entt::entity World::spawnPlayer(vec3 pos) {
	if (registry.valid(player))
		registry.destroy(this->player);
	
	this->player = spawnDefaultEntity(pos);
	registry.emplace<CKeyboardControllable>(this->player, 0.003f);
	TiledTexture *playertex = assetManager.getTiledTexture("res/images/sprites/guy_stand_frames.png", 16, 16, 0, 0);
	registry.emplace_or_replace<CBillboard>(this->player,
		playertex, 
		vec2(0.2f, 0.2f), vec3(0.961f, 0.8f, 0.545f));
	registry.get<CBillboard>(this->player).setSubRect(1.0f * 16.0f, 0.0f * 16.0f,
		16.0f, 16.0f, 48, 16);
	
	registry.remove<CJumpTimer>(this->player);
	
	registry.emplace_or_replace<CSpawnPoint>(this->player, vec3(0.0f, 0.5f, 0.0f));
	registry.emplace_or_replace<CHealth>(this->player, 12);
	
	// world pos crosshair
	worldCrosshair = registry.create();
	registry.emplace<CPosition>(worldCrosshair, vec3(0.0f));
	// TODO: assetManager.newTiledTexture("res/...", tile_x, tile_y, tileset_width, tileset_height)
	//registry.emplace<CBillboard>(worldCrosshair, assetManager.getTexture("res/images/sprites/arrows.png"),
	//	vec2(0.2f, 0.2f), vec3(0.0f, 1.0f, 0.0f));
	TiledTexture *tiledtex = assetManager.getTiledTexture("res/images/sprites/arrows.png", 64, 64, 0, 0);
	registry.emplace<CBillboard>(worldCrosshair, tiledtex, vec2(0.4f, 0.4f));
	
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
	// update systems
	charControllerSystem.update(registry, -camera->getToTarget());
	
	#if CFG_DEBUG
		Benchmark update_benchmark;
		static std::unordered_map<const char *, std::vector<double>> exec_time;
	#endif

	for (auto &sys : updateSystems) {
		#if CFG_DEBUG
			update_benchmark.start();
		#endif

		sys->update();
		
		#if CFG_DEBUG
			update_benchmark.stop();
			if (exec_time.find(typeid(*sys).name()) == exec_time.end()) {
				exec_time[typeid(*sys).name()] = std::vector<double>();
			}
			exec_time[typeid(*sys).name()].push_back(update_benchmark.ms());
		#endif
	}
	
	#if CFG_DEBUG
		if (ImGui::Begin("Systems")) {
			using namespace ImGui;
			static int vals = 60;
			InputInt("last x values", &vals, 1);

			Columns(4);
			Text("SYSTEM"); NextColumn();
			Text("t"); NextColumn();
			Text("avg"); NextColumn();
			Text("min/max"); NextColumn();

			for (auto &it : exec_time) {
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

	#if CFG_DEBUG
		if (!registry.valid(player) && this->is_loaded()) {
			spawnPlayer();
			registry.ctx<entt::dispatcher>().trigger<LogEvent>("World: respawned player because debug is enabled.", LogEvent::WARN);
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
			ivec2 chunkPos = chunkedWorld.getTerrain().worldPosToChunk(pos);
			SignedDistTerrain *e = (SignedDistTerrain *)chunkedWorld.getTerrain().getChunkGrid().at(chunkPos);
			if (!e) {
				e = new SignedDistTerrain();
			}
			
			if (vtype == vsphere) {
				e->sphere(pos, r.x, (SignedDistTerrain::Op)vop);
			} else if (vtype == vcube) {
				e->box(pos, r, (SignedDistTerrain::Op)vop);
			}
			chunkedWorld.update(chunkPos, e);
		}
	}
	ImGui::End();
	
	//BM_START(rendering, 30);
	drawFloor();
	for (auto &sys : renderSystems) sys->draw();
	//BM_STOP(rendering);
	
}


void World::setCamera(std::shared_ptr<Camera> camera) {
	this->camera = camera;
	for (auto &sys : renderSystems) sys->setCamera(camera);
}

///////////////
/// PRIVATE ///
///////////////

void World::loadSystems() {
	// clear all
	updateSystems.clear();
	
	// register a logging event system
	logEventSystem = std::make_unique<LogSystem>(registry);
	logEventSystem->setLogger([](const LogEvent &e) {
		std::cout << e.text << std::endl;
	});
	
	// initialize update and render systems
	auto billboardRenderSystem = std::make_shared<BillboardRenderSystem>(registry, camera);
	auto textRenderSystem = std::make_shared<TextEventSystem>(registry, camera);
	auto wayfindSystem = std::make_shared<WayfindSystem>(registry, camera);
	auto primitiveRenderer = std::make_shared<PrimitiveRenderSystem>(registry, camera);
	
	// create update systems
	updateSystems.emplace_back(new GravitySystem(registry, 0.000981f, tileGrid));
	updateSystems.emplace_back(new RandomJumpSystem(registry, 0.003f));
	updateSystems.push_back(wayfindSystem);
	updateSystems.emplace_back(new PressAwaySystem(registry));
	updateSystems.emplace_back(new PositionUpdateSystem(registry));
	updateSystems.push_back(billboardRenderSystem);
	updateSystems.push_back(textRenderSystem);
	updateSystems.push_back(primitiveRenderer);
	updateSystems.emplace_back(new AudioSystem(registry, assetManager));
	
	// and render systems
	renderSystems.push_back(billboardRenderSystem);
	renderSystems.push_back(textRenderSystem);
	renderSystems.push_back(wayfindSystem);
	renderSystems.push_back(primitiveRenderer);
}

void World::setupFloor() {
	
	// mapgen
	int x = 0, y = 0;
	Random r;
	const int gen_n_chunks = 4;
	for (int i = 0; i < gen_n_chunks; ++i) {
		if (tileGrid.at(x, y) == nullptr) {
			SignedDistTerrain *sd = new SignedDistTerrain();
			tileGrid.set(x, y, (SignedDistTerrain *)1);
			sd->plane(vec3(0.f), vec3(0.f, 1.f, 0.f), 0.f);
			float rand = r();
			if (rand < .2f) {
				sd->sphere(vec3(0.f), 0.4f);
				sd->sphere(vec3(0.f, .5f, 0.f), 0.4f);
			} else if (rand < .4f) {
				sd->box(vec3(0.f), vec3(0.8f), SignedDistTerrain::Op::DIFF);
			} else {
				sd->sphere(vec3(.0f, .3f, .0f), 0.7f, SignedDistTerrain::Op::DIFF);
			}
			
			chunkedWorld.set(ivec2(x, y), sd);
		}
		auto rng = r();
		if (rng < 0.25) x--;
		else if (rng < 0.5) x++;
		else if (rng < 0.75) y--;
		else y++;
		if (tileGrid.at(x, y) != nullptr) i--;
		else {
			for (int j = 0; j < int(r() * 4.f); ++j) {
				spawnDefaultEntity(vec3(x * 2.0f, 0.3f, y * 2.0f));
			}
		}
	}
	
	double starttime = glfwGetTime();
	size_t i = 0;
	chunkedWorld.getTerrain().getChunkGrid().each([this, &i, gen_n_chunks](int x, int y, Terrain *terrain) {
		registry.ctx<entt::dispatcher>().trigger<LogEvent>("World: Generating chunk " + std::to_string(i++) + "/" + std::to_string(gen_n_chunks), LogEvent::LOG);
		this->chunkedWorld.polygonizeChunk(ivec2(x, y));
	});
	#if CFG_DEBUG
		double endtime = glfwGetTime();
		registry.ctx<entt::dispatcher>().trigger<LogEvent>("World: Generated chunks in " + std::to_string(endtime - starttime) + "s.", LogEvent::LOG);
	#endif
	
}

void World::destroyFloor() {
	chunkedWorld.destroy();
}

void World::drawFloor() {
	chunkShader["uProj"] = camera->getProjection();
	chunkShader["uView"] = camera->getView();
	chunkShader["uModel"] = glm::mat4(1.f);
	chunkShader["uTextureTopdownScale"] = 2.0f;
	chunkShader["uTextureSideScale"] = 2.0f;
	chunkShader["uTextureTopdown"] = 0;
	chunkShader["uTextureSide"] = 1;
	chunkShader["uTime"] = (float)glfwGetTime();
	
	glActiveTexture(GL_TEXTURE0);
	assetManager.getTexture("res/images/textures/floor.png")->setWrapMode(Texture::WrapMode::REPEAT);
	assetManager.getTexture("res/images/textures/floor.png")->bind();
	
	glActiveTexture(GL_TEXTURE1);
	assetManager.getTexture("res/images/textures/wall.png")->setWrapMode(Texture::WrapMode::REPEAT);
	assetManager.getTexture("res/images/textures/wall.png")->bind();
	
	chunkedWorld.draw(chunkShader);
	
}
