#include "ecs/systems/TerrainRenderSystem.hpp"

TerrainRenderSystem::TerrainRenderSystem(const entt::registry &registry, std::shared_ptr<Camera> camera, AssetManager &assetManager, std::shared_ptr<ChunkedWorld> chunkedWorld)
	: IRenderSystem(registry, camera), m_assetManager{assetManager}, m_chunkedWorld{chunkedWorld}
{
	
	loadShader();

}

TerrainRenderSystem::~TerrainRenderSystem() {

}

void TerrainRenderSystem::draw() {
	updateUniforms();
	bindTextures();
	
	m_chunkedWorld->draw(m_chunkShader);
}

/////////////
// PRIVATE //
/////////////

void TerrainRenderSystem::loadShader() {
	m_chunkShader.load("res/glsl/proto/terrain_vert.glsl", sgl::shader::VERTEX);
	m_chunkShader.load("res/glsl/proto/terrain_frag.glsl", sgl::shader::FRAGMENT);
	m_chunkShader.compile();
	m_chunkShader.link();
	Blackboard::write("chunkShader", &m_chunkShader);
}

void TerrainRenderSystem::updateUniforms() {
	m_chunkShader["uProj"] = camera->getProjection();
	m_chunkShader["uView"] = camera->getView();
	m_chunkShader["uModel"] = glm::mat4(1.f);
	m_chunkShader["uTextureTopdownScale"] = 2.0f;
	m_chunkShader["uTextureSideScale"] = 2.0f;
	m_chunkShader["uTextureTopdown"] = 0;
	m_chunkShader["uTextureSide"] = 1;
	m_chunkShader["uTime"] = (float)glfwGetTime();
}

void TerrainRenderSystem::bindTextures() {
	// bind textures
	glActiveTexture(GL_TEXTURE0);
	m_assetManager.getTexture("res/images/textures/floor.png")->setWrapMode(Texture::WrapMode::REPEAT);
	m_assetManager.getTexture("res/images/textures/floor.png")->bind();
	
	glActiveTexture(GL_TEXTURE1);
	m_assetManager.getTexture("res/images/textures/wall.png")->setWrapMode(Texture::WrapMode::REPEAT);
	m_assetManager.getTexture("res/images/textures/wall.png")->bind();
}
