#include <World.hpp>

extern GLFWwindow *window;

static entt::entity spawnEntity(entt::registry &registry, glm::vec3 pos) {
	static Random rand;
	
	glm::vec3 rdir = glm::normalize(glm::vec3(
		rand() * 2.0f - 1.0f, 0.0f,
		rand() * 2.0f - 1.0f)) * 0.0025f;
	
	glm::vec2 rsize(rand() * 0.02f + 0.065f, rand() * 0.04f + 0.07f);
	
	auto entity = registry.create();
	registry.assign<CPosition>(entity, pos);
	registry.assign<CVelocity>(entity, rdir);
	registry.assign<CBillboard>(entity, rsize);
	registry.assign<CGravity>(entity);
}

World::World()
	: billboard("res/images/sprites/default_soldier_s.png")
{
	setupFloor();
	
	Random rand(-0.45f, 0.45f), randHeight(0.0f, 2.0f);
	for (int i = 0; i < 100; ++i) {
		spawnEntity(registry, glm::vec3(rand(), randHeight(), rand()));
	}
}

World::~World() {
	destroyFloor();
	registry.reset();
}

void World::update() {
	static RandomJumpSystem popcorn;
	gravitySystem.update(registry);
	popcorn.update(registry);
	
	registry.view<CPosition, CVelocity>().each([](auto entity, auto &pos, auto &vel) {
		pos.pos += vel.vel;
	});
}

void World::draw(glm::mat4 &uView, glm::mat4 &uProjection) {
	drawFloor(uView, uProjection);
	
	// render systems
	BillboardRender::draw(registry, billboard, uView, uProjection);
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
	floorShader["uModel"] = glm::mat4(1.0f);
	floorShader["uView"] = uView;
	floorShader["uProjection"] = uProjection;
	
	floorShader.use();
	glBindVertexArray(floorVAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}
