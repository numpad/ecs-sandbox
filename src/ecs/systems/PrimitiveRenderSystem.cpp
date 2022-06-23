#include <ecs/systems/PrimitiveRenderSystem.hpp>

PrimitiveRenderSystem::PrimitiveRenderSystem(entt::registry& registry, std::shared_ptr<Camera> camera)
    : IUpdateSystem(registry), IRenderSystem(registry, camera) {

	// register
	registry.ctx<entt::dispatcher>().sink<DrawPrimitiveEvent>().connect<&PrimitiveRenderSystem::receive>(*this);

	// generate VAOs for debug rendering
	glGenVertexArrays(1, &lineVAO);
	glGenBuffers(1, &lineVBO);

	glBindVertexArray(lineVAO);
	glBindBuffer(GL_ARRAY_BUFFER, lineVBO);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// load shader
	bool ok = true;
	ok &= lineShader.load("res/glsl/primitives/lines_vert.glsl", sgl::shader::VERTEX);
	ok &= lineShader.load("res/glsl/primitives/lines_frag.glsl", sgl::shader::FRAGMENT);
	ok &= lineShader.compile();
	ok &= lineShader.link();
	if (!ok)
		printf("ERROR LOADING SHADER\n");
}

PrimitiveRenderSystem::~PrimitiveRenderSystem() {
	glDeleteVertexArrays(1, &lineVAO);
	glDeleteBuffers(1, &lineVBO);
}

void PrimitiveRenderSystem::receive(const DrawPrimitiveEvent& event) {
}

void PrimitiveRenderSystem::update(float dt) {
}

void PrimitiveRenderSystem::draw() {
}
