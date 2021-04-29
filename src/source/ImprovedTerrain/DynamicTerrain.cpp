#include "ImprovedTerrain/DynamicTerrain.hpp"

////////////
// PUBLIC //
////////////

void DynamicTerrain::draw(sgl::shader *chunkshader) {
	m_chunkmeshes.each([chunkshader](glm::ivec3 chunk_idx, Mesh *mesh) {
		if (mesh)
			mesh->draw(*chunkshader);
	});
}

void DynamicTerrain::add_body(ISignedDistanceBody *body) {
	glm::imat2x3 affected = m3d::get_affected_chunks(body->get_bounding_box(), m_chunksize);
	int debug_count = 0;
	Benchmark debug_b;
	m_chunks.each_inside(affected, [this, body, &debug_count](glm::ivec3 chunk_idx, ISignedDistanceBody *sdf) {
		if (sdf == nullptr) {
			m_chunks.set(chunk_idx, body);
		} else {
			ISignedDistanceBody *previous_chunk = m_chunks.pop(chunk_idx);
			ISignedDistanceBody *new_chunk = new CSGNode(previous_chunk, body, CSGNode::Operator::UNION);
			m_chunks.set(chunk_idx, new_chunk);
		}
		std::cout << "setting " << chunk_idx.x << ",\t" << chunk_idx.y << ",\t" << chunk_idx.z << std::endl;
		++debug_count;
		polygonize(chunk_idx);
	});
	debug_b.stop();
	std::cout << "Took " << debug_b.ms() << "ms for " << debug_count << " chunks." << std::endl;
}

void DynamicTerrain::sub_body(ISignedDistanceBody *body) {
	glm::mat2x3 aabb = body->get_bounding_box();
	// increase the bounding box by a percentage of the Terrains voxel size to fix bugs near chunk borders
	aabb[0] -= m_chunkdetail * 0.1f;
	aabb[1] += m_chunkdetail * 0.1f;

	glm::imat2x3 affected = m3d::get_affected_chunks(aabb, m_chunksize);
	int debug_count = 0;
	Benchmark debug_b;
	m_chunks.each_inside(affected, [this, body, &debug_count](glm::ivec3 chunk_idx, ISignedDistanceBody *sdf) {
		if (sdf) {
			ISignedDistanceBody *previous_chunk = m_chunks.pop(chunk_idx);
			ISignedDistanceBody *new_chunk = new CSGNode(previous_chunk, body, CSGNode::Operator::DIFF);
			m_chunks.set(chunk_idx, new_chunk);
		}
		std::cout << "setting " << chunk_idx.x << ",\t" << chunk_idx.y << ",\t" << chunk_idx.z << std::endl;
		++debug_count;
		polygonize(chunk_idx); // TODO: if this didn't produce any vertices we can just delete the whole chunk?
	});
	debug_b.stop();
	std::cout << "Took " << debug_b.ms() << "ms for " << debug_count << " chunks." << std::endl;
}

///////////////
// PROTECTED //
///////////////

float DynamicTerrain::signed_distance_function(glm::vec3 world_pos) const {
	glm::ivec3 chunk_idx = glm::ivec3(glm::floor(world_pos / m_chunksize));
	ISignedDistanceFunction *sdf = m_chunks.at(chunk_idx);
	if (sdf) return sdf->get_distance(world_pos);

	return 1.0f; // TODO: what to return for empty chunk?
}

/////////////
// PRIVATE //
/////////////

void DynamicTerrain::polygonize(glm::ivec3 chunk_idx) {
	ISignedDistanceFunction *chunk = m_chunks.at(chunk_idx);
	Mesh *existing_chunk_mesh = m_chunkmeshes.at(chunk_idx);
	
	// Remove old mesh
	if (existing_chunk_mesh != nullptr) {
		delete m_chunkmeshes.pop(chunk_idx);
	}

	// No chunk data for mesh
	if (chunk == nullptr) return;
	
	glm::vec3 min = glm::vec3(
		chunk_idx.x * m_chunksize.x,
		chunk_idx.y * m_chunksize.y,
		chunk_idx.z * m_chunksize.z
	);
	glm::vec3 max = glm::vec3(
		chunk_idx.x * m_chunksize.x + m_chunksize.x,
		chunk_idx.y * m_chunksize.y + m_chunksize.y,
		chunk_idx.z * m_chunksize.z + m_chunksize.z
	);
	m_cubemarcher.setSampleRange(min, max);
	m_cubemarcher.setSampleDetail(m_chunkdetail);

	auto vertices = m_cubemarcher.polygonize(*chunk); // TODO: what happens when vertices is empty?
	if (vertices.size() == 0) std::cout << "ZERO ZERO\nZERO ZERO\n" << std::endl;
	Mesh *mesh = new Mesh(vertices, false);

	m_chunkmeshes.set(chunk_idx, mesh);

}