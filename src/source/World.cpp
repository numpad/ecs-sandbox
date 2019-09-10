#include <World.hpp>

extern GLFWwindow *window;

World::World()
	: charControllerSystem(window)
{
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
	
	glm::vec2 rsize(rand() * 0.02f + 0.065f, rand() * 0.04f + 0.07f);
	
	glm::vec3 rcol(rand() * 0.5f + 0.5f, rand() * 0.5f + 0.5f, rand() * 0.5f + 0.5f);
	
	auto entity = registry.create();
	registry.assign<CPosition>(entity, pos);
	registry.assign<CVelocity>(entity, rdir);
	registry.assign<CBillboard>(entity, rsize, rcol);
	registry.assign<CGravity>(entity);
	//registry.assign<CSphereCollider>(entity, 0.045f, 0.01f);
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
		glm::vec2(0.12f, 0.14f), glm::vec3(0.961f, 0.8f, 0.545f));
	
	registry.remove<CJumpTimer>(this->player);
	
	registry.assign_or_replace<CSpawnPoint>(this->player, glm::vec3(0.0f, 0.5f, 0.0f));
	
	return this->player;
}

void World::update(glm::vec3 viewPos, glm::vec3 viewDir) {	
	gravitySystem.update(registry, tileGrid);
	popcorn.update(registry);
	wayfindSystem.update(registry);
	charControllerSystem.update(registry, viewDir);
	pressawaySystem.update(registry);
	//billboardOrient.update(registry, ...);
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
		if (ImGui::Begin("render")) {
			if (ImGui::Button("Reset"))
				registry.reset();
		}
		ImGui::End();
	#endif
	
	// render systems
	drawFloor(uView, uProjection);
	
	// draw billboards
	
	// sort only every N ticks
	const int maxTicksUntilSort = 6;
	static int ticksSinceLastSort = 0;
	if (++ticksSinceLastSort > maxTicksUntilSort) {
		billboardSystem.depthSort(registry, camPos);
		ticksSinceLastSort = 0;
	}
	
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
	
	tileGrid.set(0, 0, assetManager.getModel("res/models/world/dungeon_floor_wall1.blend"));
	tileGrid.set(1, 0, assetManager.getModel("res/models/world/dungeon_floor_wall2_corner_nopillar.blend"));
	tileGrid.set(1, 1, assetManager.getModel("res/models/world/dungeon_floor.blend"));
	
	// world pos crosshair
	worldCrosshair = registry.create();
	registry.assign<CPosition>(worldCrosshair, glm::vec3(0.0f));
	//registry.assign<CBillboard>(worldCrosshair, glm::vec2(0.03f, 0.25f), glm::vec3(0.0f, 1.0f, 0.0f));
	
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
