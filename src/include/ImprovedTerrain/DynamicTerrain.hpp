#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/component_wise.hpp>

#include <ImprovedTerrain/ISignedDistanceFunction.hpp>
#include <ImprovedTerrain/ISignedDistanceBody.hpp>
#include <ImprovedTerrain/SparseGrid3D.hpp>
#include <ImprovedTerrain/CSGTree.hpp>
#include <ImprovedTerrain/Bodies/SphereBody.hpp>
#include <ImprovedTerrain/Bodies/CubeBody.hpp>
#include <ImprovedTerrain/Bodies/DiskBody.hpp>
#include <Terrain/CubeMarcher.hpp>
#include <Assets/Mesh.hpp>
#include <Assets/AssetManager.hpp>
#include <RenderObject/Camera.hpp>
#include <Util/Math3d.hpp>
#include <Util/Benchmark.hpp>
#include <iostream>

class DynamicTerrain : public ISignedDistanceFunction {
public:

	DynamicTerrain() {
		ISignedDistanceBody *s = new DiskBody(glm::vec3(0.f), 2.8f, 0.1f);

		glm::imat2x3 affected = m3d::get_affected_chunks(s->get_bounding_box(), m_chunksize);
		m_chunks.each_inside(affected, [this, s](glm::ivec3 chunk_idx, ISignedDistanceBody *sdf) {
			m_chunks.set(chunk_idx, s);
			std::cout << "setting " << chunk_idx.x << ",\t" << chunk_idx.y << ",\t" << chunk_idx.z << std::endl;
		});

		Benchmark b;
		m_chunks.each([this](glm::ivec3 chunk_idx, ISignedDistanceBody *) { polygonize(chunk_idx); });
		b.stop();
		std::cout << "took " << b.ms() << "ms" << std::endl;
	}
	~DynamicTerrain() {
		// TODO: delete all in m_chunks?
		m_chunkmeshes.each([](glm::ivec3 chunk_idx, Mesh *mesh) { delete mesh; });
	}

	void draw(sgl::shader *chunkshader);

	void add_body(ISignedDistanceBody *body);
	void sub_body(ISignedDistanceBody *body);

protected:
	float signed_distance_function(glm::vec3 world_pos) const;


private:
	void polygonize(glm::ivec3 chunk_idx);

	SparseGrid3D<ISignedDistanceBody> m_chunks;
	SparseGrid3D<Mesh> m_chunkmeshes;
	glm::vec3 m_chunksize = glm::vec3(1.0f);
	float m_chunksize_min = glm::compMin(m_chunksize);
	float m_chunkdetail = 0.05f;
	
	CubeMarcher m_cubemarcher;

	void set_chunksize(glm::vec3 size) {
		m_chunksize = size;
		m_chunksize_min = glm::compMin(size);
	}
};
