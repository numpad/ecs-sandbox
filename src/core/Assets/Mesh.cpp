#include <Assets/Mesh.hpp>

using namespace glm;

Mesh::Mesh(std::vector<Vertex> vertices, bool hasTexcoords)
    : vertices(vertices), hasTexcoords(hasTexcoords), hasIndices(false) {
	setupMesh();
}

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<Texture*> textures, bool hasTexcoords)
    : vertices(vertices), textures(textures), hasTexcoords(hasTexcoords), hasIndices(false) {
	setupMesh();
}

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, bool hasTexcoords)
    : vertices(vertices), indices(indices), hasTexcoords(hasTexcoords), hasIndices(true) {
	setupMesh();
}

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<Texture*> textures, bool hasTexcoords)
    : vertices(vertices), indices(indices), textures(textures), hasTexcoords(hasTexcoords), hasIndices(true) {
	setupMesh();
}

Mesh::~Mesh() {
	this->destroy();
}

void Mesh::draw(sgl::shader& shader) {
	GLuint iDiffuse = 0;
	GLuint iSpecular = 0;
	GLuint iNormal = 0;

	shader.use();

	for (auto it = textures.begin(); it != textures.end(); ++it) {
		Texture* tex = *it;
		GLint index = (GLuint)std::distance(textures.begin(), it);

		GLuint texIdx = 0;
		switch (tex->getUsageType()) {
		case Texture::UsageType::DIFFUSE:
			texIdx = iDiffuse++;
			break;
		case Texture::UsageType::SPECULAR:
			texIdx = iSpecular++;
			break;
		case Texture::UsageType::NORMAL:
			texIdx = iNormal++;
			break;
		};

		std::string textureName = "uTex" + tex->getUsageString() + std::to_string(texIdx);

		// set uniform to active texture
		glActiveTexture(GL_TEXTURE0 + index);
		shader[textureName.c_str()] = index;
		tex->bind();
	}

	glBindVertexArray(VAO);
	if (hasIndices)
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, (void*)0);
	else
		glDrawArrays(GL_TRIANGLES, 0, vertices.size());
	glBindVertexArray(0);
}

void Mesh::destroy() {
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

/////////////
// PRIVATE //
/////////////

void Mesh::setupMesh() {
	// send to gpu
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
	if (hasIndices) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);
	}
	// position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
	// normal
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	// color
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
	// texcoord
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texcoords));

	glBindVertexArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
