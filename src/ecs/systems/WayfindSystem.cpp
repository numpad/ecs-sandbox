#include <ecs/systems/WayfindSystem.hpp>

WayfindSystem::WayfindSystem(entt::registry &registry, std::shared_ptr<Camera> camera)
	: BaseUpdateSystem(registry), BaseRenderSystem(registry, camera) {
	
	// generate VAOs for debug rendering
	glGenVertexArrays(1, &lineVAO);
	glGenBuffers(1, &lineVBO);
	
	glBindVertexArray(lineVAO);
	glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
	
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void *)0);
	
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

WayfindSystem::~WayfindSystem() {
	glDeleteVertexArrays(1, &lineVAO);
	glDeleteBuffers(1, &lineVBO);
}

void WayfindSystem::update() {
	registry.view<CPosition, CRunningToTarget>().each([&registry = registry]
		(auto &entity, auto &pos, auto &runToTarget) {
			glm::vec3 dirToTarget = runToTarget.getTargetPosition(registry) - pos.pos;
			
			if (glm::length(dirToTarget) > runToTarget.closeEnough) {
				if (registry.has<CVelocity>(entity)) {
					auto &vel = registry.get<CVelocity>(entity);
					if (glm::length(vel.vel) < 0.009f)
						vel.acc += glm::normalize(dirToTarget) * runToTarget.force;
				} else {
					pos.pos += glm::normalize(dirToTarget) * runToTarget.force;
				}
			} else { // reached target
				if (runToTarget.stopOnceReached) {
					registry.remove<CRunningToTarget>(entity);
					// look a little more alive
					registry.emplace_or_replace<CJumpTimer>(entity);
				}
			}
		});
}

void WayfindSystem::draw() {
	std::vector<vec3> lines;
	registry.view<CPosition, CRunningToTarget>().each([&registry = registry, &lines](auto &entity, auto &pos, auto &runToTarget) {
		lines.push_back(pos.pos);
		lines.push_back(runToTarget.getTargetPosition(registry));
	});
	
	glBindVertexArray(lineVAO);
	glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
	glBufferData(GL_ARRAY_BUFFER, lines.size() * sizeof(GLfloat) * 3, &lines[0], GL_STATIC_DRAW);
	
	lineShader["uProjection"] = camera->getProjection();
	lineShader["uView"] = camera->getView();
	lineShader["uColor"] = vec3(1.0, 0.0, 0.0);
	lineShader.use();
	
	glDrawArrays(GL_LINES, 0, lines.size());
	
}