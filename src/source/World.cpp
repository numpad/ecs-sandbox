#include <World.hpp>

using namespace glm;

extern GLFWwindow *window;

World::World()
	: chunks(vec3(2.f)), charControllerSystem(window)
{
	registry.set<entt::dispatcher>();
	
	// load systems
	loadSystems();
	
	// load world
	setupFloor();
	
	// spawn player
	spawnPlayer();
	
	
}

World::~World() {
	destroyFloor();
	registry.reset();
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
	registry.assign<CPosition>(entity, pos);
	registry.assign<CVelocity>(entity, vec3(0.f));
	registry.assign<CBillboard>(entity,
		this->assetManager.getTexture("res/images/textures/dungeon.png"), rsize, rcol);
	registry.get<CBillboard>(entity).setSubRect(1.0f * 16.0f, 10.0f * 16.0f, 16.0f, 16.0f, 256, 256);
	registry.assign<CGravity>(entity);
	registry.assign<CSphereCollider>(entity, 0.045f, 0.01f);
	registry.assign<CJumpTimer>(entity, 0);
	registry.assign<CHealth>(entity, 10);
	if (registry.valid(this->player)) {
		//registry.assign<CRunningToTarget>(entity, this->player, 0.001f, 0.2f);
	}
	return entity;
}

entt::entity World::spawnPlayer(vec3 pos) {
	if (registry.valid(player))
		registry.destroy(this->player);
	
	this->player = spawnDefaultEntity(pos);
	registry.assign<CKeyboardControllable>(this->player, 0.003f);
	TiledTexture *playertex = assetManager.getTiledTexture("res/images/sprites/guy_stand_frames.png", 16, 16, 0, 0);
	registry.assign_or_replace<CBillboard>(this->player,
		playertex, 
		vec2(0.2f, 0.2f), vec3(0.961f, 0.8f, 0.545f));
	registry.get<CBillboard>(this->player).setSubRect(1.0f * 16.0f, 0.0f * 16.0f,
		16.0f, 16.0f, 48, 16);
	
	registry.remove<CJumpTimer>(this->player);
	
	registry.assign_or_replace<CSpawnPoint>(this->player, vec3(0.0f, 0.5f, 0.0f));
	registry.assign_or_replace<CHealth>(this->player, 12);
	
	// world pos crosshair
	worldCrosshair = registry.create();
	registry.assign<CPosition>(worldCrosshair, vec3(0.0f));
	// TODO: assetManager.newTiledTexture("res/...", tile_x, tile_y, tileset_width, tileset_height)
	//registry.assign<CBillboard>(worldCrosshair, assetManager.getTexture("res/images/sprites/arrows.png"),
	//	vec2(0.2f, 0.2f), vec3(0.0f, 1.0f, 0.0f));
	TiledTexture *tiledtex = assetManager.getTiledTexture("res/images/sprites/arrows.png", 64, 64, 0, 0);
	registry.assign<CBillboard>(worldCrosshair, tiledtex, vec2(0.4f, 0.4f));
	return this->player;
}

void World::update(vec3 viewPos, vec3 viewDir) {
	// update systems
	charControllerSystem.update(registry, viewDir);
	for (auto &sys : updateSystems) sys->update();
	
	#if CFG_DEBUG
		if (!registry.valid(player)) {
			spawnPlayer();
			printf("[LOG] World: respawned player because debug is enabled.\n");
		}
	#endif
}

void World::draw(vec3 &camPos, mat4 &uView, mat4 &uProjection) {
	#if CFG_IMGUI_ENABLED
		if (ImGui::Begin("entities", NULL, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar)) {
			using namespace ImGui;
			using namespace glm;
			if (Button("Reset")) {
				registry.reset();
				spawnPlayer();
			}
			// minimap
			ImVec2 winsize = GetContentRegionAvail();
			const mat3 transform = mat3(
				vec3(winsize.x, 0.0f, 0.0f),
				vec3(0.0f, winsize.y, 0.0f),
				vec3(winsize.x * 0.5f, winsize.y * 0.5f, 1.0f));
			vec3 originP = vec3(0.0f, 0.0f, 1.0f);
			if (registry.valid(getPlayer())) {
				originP.x = registry.get<CPosition>(getPlayer()).pos.x;
				originP.y = registry.get<CPosition>(getPlayer()).pos.z;
			}
			vec2 origin = vec2(transform * originP);
			
			auto drawlist = GetWindowDrawList();
			auto wp = GetCursorScreenPos(); // offset of display top left to window
			const float s = 0.1f; // scale
			
			registry.view<CPosition, CSphereCollider, CBillboard>().each([&](auto entity, auto &pos, auto &sphere, auto &bb){
				const vec2 op = vec2(transform * vec3(pos.pos.x * s, pos.pos.z * s, 1.0));
				float radius = sphere.radius * 50.0f;
				ImColor color = ImColor(1.0f, 0.0f, 0.0f);
				if (registry.has<CKeyboardControllable>(entity)) {
					color = ImColor(0.0f, 0.0f, 1.0f);
					vec2 camdir = vec2(pos.pos.x, pos.pos.z) - vec2(camPos.x, camPos.z);
					drawlist->AddLine(ImVec2(wp.x + op.x - origin.x * s, wp.y + op.y - origin.y * s),
						ImVec2(wp.x + op.x + normalize(camdir).x * 35.0f - origin.x * s, wp.y + op.y + normalize(camdir).y * 35.0f - origin.y * s),
						ImColor(1.0f, 1.0f, 0.0f));
				}
				drawlist->AddCircleFilled(ImVec2(wp.x + op.x - origin.x * s, wp.y + op.y - origin.y * s), radius, color, 12);
			});
		}
		ImGui::End();
	#endif
	
	// render systems
	drawFloor(uView, uProjection);
	
	// draw billboards
	
	// sort only every N ticks
	const int maxTicksUntilSort = 2;
	static int ticksSinceLastSort = 0;
	if (++ticksSinceLastSort > maxTicksUntilSort) {
		
		ticksSinceLastSort = 0;
	}
	#ifdef CFG_DEBUG
		double st = glfwGetTime();
	#endif
	
	billboardRenderSystem->depthSort(registry, camPos);
	
	#ifdef CFG_DEBUG
		double ms = ((glfwGetTime() - st) * 1000.f);
		static double ms_total = 0.;
		ms_total += ms;
		static int cnt = 0, max_cnt = 30;
		if (++cnt >= max_cnt) {
			printf("%.2fms for depthsort over %d ticks\n", ms_total / (double)cnt, cnt);
			cnt = 0;
			ms_total = 0.;
		}
	#endif
		
	billboardRenderSystem->drawInstanced(registry, uView, uProjection);
	
}


///////////////
/// PRIVATE ///
///////////////

void World::loadSystems() {
	// clear all
	updateSystems.clear();
	
	// create update systems
	updateSystems.emplace_back(new GravitySystem(registry, 0.000981f, tileGrid));
	updateSystems.emplace_back(new RandomJumpSystem(registry, 0.003f));
	updateSystems.emplace_back(new WayfindSystem(registry));
	updateSystems.emplace_back(new PressAwaySystem(registry));
	updateSystems.emplace_back(new PositionUpdateSystem(registry));
	
	// and render systems
	billboardRenderSystem = std::make_unique<BillboardRenderSystem>(registry);
}

void World::setupFloor() {
	chunkShader.load("res/glsl/proto/terrain_vert.glsl", sgl::shader::VERTEX);
	chunkShader.load("res/glsl/proto/terrain_frag.glsl", sgl::shader::FRAGMENT);
	chunkShader.compile();
	chunkShader.link();
	
	// mapgen
	int x = 0, y = 0;
	Random r;
	const int gen_n_chunks = 2;
	for (int i = 0; i < gen_n_chunks; ++i) {
		// set model
		tileGrid.set(x, y, new SignedDistTerrain());
		SignedDistTerrain *sd = tileGrid.at(x, y);
		sd->plane(vec3(0.f), vec3(0.f, 1.f, 0.f), 0.f);
		if (r() > 0.5)
			sd->sphere(vec3(0.f), 0.4f);
		chunks.set(ivec2(x, y), *sd);
		
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
	#if CFG_DEBUG
		double starttime = glfwGetTime();
	#endif
	size_t i = 0;
	for (auto it : chunks.getTerrain().getChunks()) {
		#if CFG_DEBUG
			printf("Generating chunk %ld/%d...\n", i++, gen_n_chunks);
		#endif
		chunks.polygonizeChunk(it.first);
	}
	#if CFG_DEBUG
		double endtime = glfwGetTime();
		printf("[LOG] World: generated %d chunks in %.2fs.\n", gen_n_chunks, endtime - starttime);
	#endif
	
}

void World::destroyFloor() {
	tileGrid.each([](int, int, SignedDistTerrain *t) { delete t; });
}

void World::drawFloor(mat4 &uView, mat4 &uProjection) {
	chunkShader["uProj"] = uProjection;
	chunkShader["uView"] = uView;
	chunkShader["uModel"] = glm::mat4(1.f);
	chunkShader["uTextureTopdownScale"] = 2.0f;
	chunkShader["uTextureSideScale"] = 2.0f;
	chunkShader["uTextureTopdown"] = 0;
	chunkShader["uTextureSide"] = 1;
	
	glActiveTexture(GL_TEXTURE0);
	assetManager.getTexture("res/images/textures/floor.png")->setWrapMode(Texture::WrapMode::REPEAT);
	assetManager.getTexture("res/images/textures/floor.png")->bind();
	
	glActiveTexture(GL_TEXTURE1);
	assetManager.getTexture("res/images/textures/wall.png")->setWrapMode(Texture::WrapMode::REPEAT);
	assetManager.getTexture("res/images/textures/wall.png")->bind();
	
	chunks.draw(chunkShader);
	
}
