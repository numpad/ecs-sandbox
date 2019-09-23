#include <World.hpp>

extern GLFWwindow *window;

World::World()
	: charControllerSystem(window)
{
	//auto group = registry.group<CPosition, CBillboard>();
	
	setupFloor();
	
	// spawn player
	spawnPlayer();
	
	
}

World::~World() {
	destroyFloor();
	registry.reset();
}

entt::entity World::getNearestEntity(glm::vec3 posNear) {
	entt::entity nearest = entt::null;
	
	registry.view<CPosition>().each([this, &nearest, posNear](auto entity, auto &pos) {
		if (entity != this->worldCrosshair) {
			if (nearest == entt::null) {
				nearest = entity;
				return;
			}
			
			if (glm::distance(posNear, pos.pos) < glm::distance(posNear, this->registry.get<CPosition>(nearest).pos)) {
				nearest = entity;
			}
		}
	});
	
	return nearest;
}

entt::entity World::spawnDefaultEntity(glm::vec3 pos) {
	static Random rand;
	
	glm::vec3 rdir = glm::normalize(glm::vec3(
		rand() * 2.0f - 1.0f, 0.01f,
		rand() * 2.0f - 1.0f)) * 0.0025f;
	
	glm::vec2 rsize(rand() * 0.04f + 0.2f, rand() * 0.04f + 0.2f);
	
	glm::vec3 rcol(rand() * 0.5f + 0.5f, rand() * 0.5f + 0.5f, rand() * 0.5f + 0.5f);
	
	auto entity = registry.create();
	registry.assign<CPosition>(entity, pos);
	registry.assign<CVelocity>(entity, rdir);
	registry.assign<CBillboard>(entity,
		this->assetManager.getTexture("res/images/textures/dungeon.png"), rsize, rcol);
	registry.get<CBillboard>(entity).setSubRect(1.0f * 16.0f, 10.0f * 16.0f, 16.0f, 16.0f, 256, 256);
	registry.assign<CGravity>(entity);
	registry.assign<CSphereCollider>(entity, 0.045f, 0.01f);
	registry.assign<CJumpTimer>(entity, 0);
	if (registry.valid(this->player)) {
		//registry.assign<CRunningToTarget>(entity, this->player, 0.001f, 0.2f);
	}
	return entity;
}

entt::entity World::spawnPlayer(glm::vec3 pos) {
	if (registry.valid(player))
		registry.destroy(this->player);
	
	this->player = spawnDefaultEntity(pos);
	registry.assign<CKeyboardControllable>(this->player, 0.003f);
	registry.assign_or_replace<CBillboard>(this->player,
		this->assetManager.getTexture("res/images/sprites/guy_stand_frames.png"),
		glm::vec2(0.2f, 0.2f), glm::vec3(0.961f, 0.8f, 0.545f));
	registry.get<CBillboard>(this->player).setSubRect(1.0f * 16.0f, 0.0f * 16.0f,
		16.0f, 16.0f, 48, 16);
	
	registry.remove<CJumpTimer>(this->player);
	
	registry.assign_or_replace<CSpawnPoint>(this->player, glm::vec3(0.0f, 0.5f, 0.0f));
	
	// world pos crosshair
	worldCrosshair = registry.create();
	registry.assign<CPosition>(worldCrosshair, glm::vec3(0.0f));
	//registry.assign<CBillboard>(worldCrosshair, this->assetManager.getTexture("res/images/textures/dungeon.png"),
	//	glm::vec2(0.03f, 0.25f), glm::vec3(0.0f, 1.0f, 0.0f));
	
	return this->player;
}

void World::update(glm::vec3 viewPos, glm::vec3 viewDir) {
	gravitySystem.update(registry, tileGrid);
	popcorn.update(registry);
	wayfindSystem.update(registry);
	charControllerSystem.update(registry, viewDir);
	pressawaySystem.update(registry);
	posUpdate.update(registry);
	
	#if CFG_DEBUG
		if (!registry.valid(player)) {
			spawnPlayer();
			printf("[LOG] World: respawned player because debug is enabled.\n");
		}
	#endif
}

void World::draw(glm::vec3 &camPos, glm::mat4 &uView, glm::mat4 &uProjection) {
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
	billboardSystem.depthSort(registry, camPos);
	billboardSystem.drawInstanced(registry, uView, uProjection);
	
}


///////////////
/// PRIVATE ///
///////////////

void World::setupFloor() {
	tileGridShader.load("res/glsl/proto/simpleMesh_vert.glsl", sgl::shader::VERTEX);
	tileGridShader.load("res/glsl/proto/simpleMesh_frag.glsl", sgl::shader::FRAGMENT);
	tileGridShader.compile();
	tileGridShader.link();
	
	// mapgen
	int x = 0, y = 0;
	Random r;
	Model* models[] = {
		assetManager.getModel("res/models/world/dungeon_floor.blend"),
		assetManager.getModel("res/models/world/dungeon_floor_wall1.blend"),
		assetManager.getModel("res/models/world/dungeon_floor_wall2_corner_nopillar.blend"),
		assetManager.getModel("res/models/world/dungeon_floor_wall2_opposite.blend")};
	
	for (int i = 0; i < 48; ++i) {
		
		tileGrid.set(x, y, models[size_t(r() * 2.0)]);
		auto rng = r();
		if (rng < 0.25) x--;
		else if (rng < 0.5) x++;
		else if (rng < 0.75) y--;
		else y++;
		if (tileGrid.at(x, y) != nullptr) i--;
		else {
			for (int j = 0; j < int(r() * 4.0f); ++j) {
				auto entity = spawnDefaultEntity(glm::vec3(x * 2.0f, 0.3f, y * 2.0f));
				auto &vel = registry.get<CVelocity>(entity).acc;
				vel.x = r() * 0.025f - 0.0125f;
				vel.z = r() * 0.025f - 0.0125f;
			}
		}
	}
	
	/// ---------- here
	
}

void World::destroyFloor() {
	
}

void World::drawFloor(glm::mat4 &uView, glm::mat4 &uProjection) {
	// draw worlds
	tileGridShader["uView"] = uView;
	tileGridShader["uProj"] = uProjection;
	tileGrid.each([this](int x, int y, Model *model) {
		glm::mat4 uModel = glm::mat4(1.0f);
		uModel = glm::translate(uModel, glm::vec3(x * 2.0f, 0.0f, y * 2.0f));
		uModel = glm::rotate(uModel, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		
		tileGridShader["uModel"] = uModel;
		model->draw(tileGridShader);
	});
	
	// imgui tilegrid window
	#if CFG_IMGUI_ENABLED
		using namespace ImGui;
		static int worldpos[2] = {0};
		static char modelpath[128] = "dungeon_floor";
		if (Begin("tilegrid")) {
			InputInt2("tile pos", worldpos);
			InputText("model", modelpath, 128);
			if (Button("Add Tile")) {
				tileGrid.set(worldpos[0], worldpos[1],
					assetManager.getModel("res/models/world/" + std::string(modelpath) + ".blend"));
			}
		}
		End();
	#endif
	
}
