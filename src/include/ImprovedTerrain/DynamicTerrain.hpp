#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include <ImprovedTerrain/ISignedDistanceFunction.hpp>
#include <ImprovedTerrain/ISignedDistanceBody.hpp>
#include <ImprovedTerrain/SparseGrid3D.hpp>
#include <ImprovedTerrain/CSGTree.hpp>
#include <ImprovedTerrain/SphereBody.hpp>
#include <ImprovedTerrain/CubeBody.hpp>
#include <Terrain/CubeMarcher.hpp>
#include <Assets/Mesh.hpp>
#include <Assets/AssetManager.hpp>
#include <RenderObject/Camera.hpp>
#include <iostream>

class DynamicTerrain : public ISignedDistanceFunction {
public:

	DynamicTerrain() {
		m_chunkshader.load("res/glsl/proto/terrain_vert.glsl", sgl::shader::VERTEX);
		m_chunkshader.load("res/glsl/proto/terrain_frag.glsl", sgl::shader::FRAGMENT);
		m_chunkshader.compile();
		m_chunkshader.link();
		
		SphereBody *s = new SphereBody(glm::vec3(0.f), 0.5f);

		m_chunks.set(glm::ivec3(0, 0, 0), s);
		m_chunks.set(glm::ivec3(-1, 0, 0), s);
		m_chunks.set(glm::ivec3(0, 0, -1), s);
		m_chunks.set(glm::ivec3(-1, 0, -1), s);
		
		m_chunks.each([this](glm::ivec3 chunk_idx, ISignedDistanceFunction *) { polygonize(chunk_idx); });
	}
	~DynamicTerrain() {
		// TODO: delete all in m_chunks
		// TODO: delete all in m_chunkmeshes
	}

	void draw(Camera &camera) {
		m_chunkshader["uProj"] = camera.getProjection();
		m_chunkshader["uView"] = camera.getView();
		m_chunkshader["uModel"] = glm::mat4(1.f);
		m_chunkshader["uTextureTopdownScale"] = 2.0f;
		m_chunkshader["uTextureSideScale"] = 2.0f;
		m_chunkshader["uTextureTopdown"] = 0;
		m_chunkshader["uTextureSide"] = 1;
		m_chunkshader["uTime"] = (float)glfwGetTime();

		glActiveTexture(GL_TEXTURE0);
		m_assetmanager.getTexture("res/images/textures/floor.png")->setWrapMode(Texture::WrapMode::REPEAT);
		m_assetmanager.getTexture("res/images/textures/floor.png")->bind();
		
		glActiveTexture(GL_TEXTURE1);
		m_assetmanager.getTexture("res/images/textures/wall.png")->setWrapMode(Texture::WrapMode::REPEAT);
		m_assetmanager.getTexture("res/images/textures/wall.png")->bind();

		m_chunkmeshes.each([this](glm::ivec3 chunk_idx, Mesh *mesh) {
			mesh->draw(m_chunkshader);
		});
	}

protected:
	float signed_distance_function(glm::vec3 world_pos) const;


private:
	void polygonize(glm::ivec3 chunk_idx);

	SparseGrid3D<ISignedDistanceFunction> m_chunks;
	SparseGrid3D<Mesh> m_chunkmeshes;
	glm::vec3 m_chunksize = glm::vec3(1.f);
	float m_chunkdetail = 0.1f;
	
	CubeMarcher m_cubemarcher;
	sgl::shader m_chunkshader;

	AssetManager m_assetmanager; // temporarily

};
