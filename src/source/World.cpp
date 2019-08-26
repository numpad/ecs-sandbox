#include <World.hpp>

extern GLFWwindow *window;

entt::entity World::spawnEntity(entt::registry &registry, glm::vec3 pos) {
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
	registry.assign<CPressAway>(entity, 0.045f, 0.01f);
	registry.assign<CJumpTimer>(entity, 20);
	if (registry.valid(this->player)) {
		registry.assign<CRunningToTarget>(entity, this->player, 0.001f, 0.2f);
	}
	return entity;
}

World::World()
	: charControllerSystem(window)
{
	setupFloor();
	
	this->player = spawnDefaultEntity(glm::vec3(0.0f));
	registry.assign<CKeyboardControllable>(this->player, 0.003f);
	registry.assign_or_replace<CBillboard>(this->player,
		glm::vec2(0.12f, 0.14f), glm::vec3(0.961f, 0.8f, 0.545f));
	registry.remove<CJumpTimer>(this->player);
}

World::~World() {
	destroyFloor();
	registry.reset();
}

entt::entity World::getNearestEntity(glm::vec3 posNear) {
	entt::entity nearest = entt::null;
	
	registry.view<CPosition>().each([this, &nearest, posNear](auto entity, auto &pos) {
		if (nearest == entt::null) {
			nearest = entity;
			return;
		}
		
		if (glm::distance(posNear, pos.pos) < glm::distance(posNear, this->registry.get<CPosition>(nearest).pos)) {
			nearest = entity;
		}
	});
	
	return nearest;
}

entt::entity World::spawnDefaultEntity(glm::vec3 pos) {
	return spawnEntity(registry, pos);
}

void World::update(glm::vec3 viewPos, glm::vec3 viewDir) {	
	gravitySystem.update(registry);
	popcorn.update(registry);
	wayfindSystem.update(registry);
	charControllerSystem.update(registry, viewPos, viewDir);
	pressawaySystem.update(registry);
	//billboardOrient.update(registry, ...);
	posUpdate.update(registry);
}

void World::draw(glm::vec3 &camPos, glm::mat4 &uView, glm::mat4 &uProjection) {
	drawFloor(uView, uProjection);
	
	// render systems
	static bool renderInstanced = true;
	if (ImGui::Begin("render")) {
		ImGui::Checkbox("instancing", &renderInstanced);
		if (ImGui::Button("Reset"))
			registry.reset();
	}
	ImGui::End();
	
	billboardSystem.depthSort(registry, camPos);
	if (renderInstanced)
		billboardSystem.drawInstanced(registry, uView, uProjection);
	else
		billboardSystem.draw(registry, uView, uProjection);
}


///////////////
/// PRIVATE ///
///////////////

void World::setupFloor() {
	/*
	Assimp::Importer importer;
	auto model_path = "res/models/floor.ply";
	const aiScene *scene = importer.ReadFile(model_path,
		aiProcess_Triangulate | aiProcess_FlipUVs);
	// model should be loaded
	assert(scene);
	assert(!(scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE));
	assert(scene->mRootNode);
	*/
	
	// loads the floor shader
	floorShader.load("res/glsl/world/floor_vert.glsl", sgl::shader::VERTEX);
	floorShader.load("res/glsl/world/floor_frag.glsl", sgl::shader::FRAGMENT);
	floorShader.compile();
	floorShader.link();
	
	// prepare floor mesh
	GLfloat vertices[] = {
		-0.5f, 0.0f, -0.5f,
		 0.5f, 0.0f, -0.5f,
		-0.5f, 0.0f,  0.5f,
		 0.5f, 0.0f,  0.5f
	};
	GLuint indices[] = {
		0, 1, 2,
		2, 1, 3
	};
	
	glGenVertexArrays(1, &floorVAO);
	glGenBuffers(1, &floorVBO);
	glGenBuffers(1, &floorEBO);
	
	glBindVertexArray(floorVAO);
	// vbo
	glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
		glEnableVertexAttribArray(0);
	// ebo
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, floorEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
}

void World::destroyFloor() {
	glDeleteVertexArrays(1, &floorVAO);
	glDeleteBuffers(1, &floorVBO);
	glDeleteBuffers(1, &floorEBO);
	floorShader.unload();
}

void World::drawFloor(glm::mat4 &uView, glm::mat4 &uProjection) {
	floorShader["uColor"] = glm::vec3(0.9f, 0.66f, 0.63f);
	floorShader["uModel"] = glm::scale(glm::mat4(1.0f), glm::vec3(4.0f));
	floorShader["uView"] = uView;
	floorShader["uProjection"] = uProjection;
	
	floorShader.use();
	glBindVertexArray(floorVAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}
