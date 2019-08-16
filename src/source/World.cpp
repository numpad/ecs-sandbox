#include <World.hpp>

extern GLFWwindow *window;

World::World()
	: grid(3)
{
	setupFloor();
	
}

World::~World() {
	destroyFloor();
}

void World::draw() {
	int iw, ih;
	glfwGetWindowSize(window, &iw, &ih);
	float aspect = (float)iw / (float)ih;
	printf("aspect = %g\n", aspect);
	glm::mat4 uProj = glm::perspective(glm::radians(30.0f), aspect, 0.1f, 100.0f);
	glm::mat4 uView = glm::lookAt(
		glm::vec3(-1.25f, 1.5f, -1.25f),
		glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	
	floorShader["uColor"] = glm::vec3(0.631f, 0.875f, 0.902f);
	floorShader["uModel"] = glm::mat4(1.0f);
	floorShader["uView"] = uView;
	floorShader["uProjection"] = uProj;
	
	floorShader.use();
	glBindVertexArray(floorVAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	
}


///////////////
/// PRIVATE ///
///////////////

void World::setupFloor() {
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
