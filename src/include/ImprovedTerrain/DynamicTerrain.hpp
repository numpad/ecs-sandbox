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
#include <Util/Math3d.hpp>
#include <Util/Benchmark.hpp>
#include <iostream>

class DynamicTerrain : public ISignedDistanceFunction {
public:

	DynamicTerrain() {		
		SphereBody *s1 = new SphereBody(glm::vec3(0.f), 0.5f);
		CubeBody *c1 = new CubeBody(glm::vec3(1.f, 0.f, 0.f), glm::vec3(1.1f, 0.5f, 0.5f));
		CSGNode *s = new CSGNode(s1, c1, CSGNode::Operator::UNION);

		glm::imat2x3 affected = m3d::get_affected_chunks(s->get_bounding_box(), m_chunksize);
		m_chunks.each_inside(affected, [this, s](glm::ivec3 chunk_idx, ISignedDistanceFunction *sdf) {
			m_chunks.set(chunk_idx, s);
			std::cout << "setting " << chunk_idx.x << ",\t" << chunk_idx.y << ",\t" << chunk_idx.z << std::endl;
		});

		Benchmark b;
		m_chunks.each([this](glm::ivec3 chunk_idx, ISignedDistanceFunction *) { polygonize(chunk_idx); });
		b.stop();
		std::cout << "took " << b.ms() << "ms" << std::endl;
	}
	~DynamicTerrain() {
		// TODO: delete all in m_chunks?
		m_chunkmeshes.each([](glm::ivec3 chunk_idx, Mesh *mesh) { delete mesh; });
	}

	void draw(sgl::shader *chunkshader) {
		m_chunkmeshes.each([this, chunkshader](glm::ivec3 chunk_idx, Mesh *mesh) {
			mesh->draw(*chunkshader);
		});
	}

	void add_body(ISignedDistanceBody *body) {
		glm::imat2x3 affected = m3d::get_affected_chunks(body->get_bounding_box(), m_chunksize);
		int debug_count = 0;
		Benchmark debug_b;
		m_chunks.each_inside(affected, [this, body, &debug_count](glm::ivec3 chunk_idx, ISignedDistanceFunction *sdf) {
			if (sdf == nullptr) {
				m_chunks.set(chunk_idx, body);
			} else {
				// TODO: this is not always a SDBody, maybe change m_chunks from SDF* to SDB* ??
				m_chunks.set(chunk_idx, new CSGNode((ISignedDistanceBody*)m_chunks.pop(chunk_idx), body, CSGNode::Operator::UNION));
			}
			std::cout << "setting " << chunk_idx.x << ",\t" << chunk_idx.y << ",\t" << chunk_idx.z << std::endl;
			++debug_count;
			polygonize(chunk_idx);
		});
		debug_b.stop();
		std::cout << "Took " << debug_b.ms() << "ms for " << debug_count << " chunks." << std::endl;
	}

	void sub_body(ISignedDistanceBody *body) {
		glm::imat2x3 affected = m3d::get_affected_chunks(body->get_bounding_box(), m_chunksize);
		int debug_count = 0;
		Benchmark debug_b;
		m_chunks.each_inside(affected, [this, body, &debug_count](glm::ivec3 chunk_idx, ISignedDistanceFunction *sdf) {
			if (sdf) {
				// TODO: this is not always a SDBody, maybe change m_chunks from SDF* to SDB* ??
				m_chunks.set(chunk_idx, new CSGNode((ISignedDistanceBody*)m_chunks.pop(chunk_idx), body, CSGNode::Operator::DIFF));
			}
			std::cout << "setting " << chunk_idx.x << ",\t" << chunk_idx.y << ",\t" << chunk_idx.z << std::endl;
			++debug_count;
			polygonize(chunk_idx); // TODO: if this didn't produce any vertices we can just delete the whole chunk?
		});
		debug_b.stop();
		std::cout << "Took " << debug_b.ms() << "ms for " << debug_count << " chunks." << std::endl;
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

};
