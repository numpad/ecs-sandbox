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
	m_chunks.each_inside(affected, [this, body](glm::ivec3 chunk_idx, ISignedDistanceBody *sdf) {
		if (sdf == nullptr) {
			m_chunks.set(chunk_idx, body);
		} else {
			ISignedDistanceBody *previous_chunk = m_chunks.pop(chunk_idx);
			ISignedDistanceBody *new_chunk = new CSGNode(previous_chunk, body, CSGNode::Operator::UNION);
			m_chunks.set(chunk_idx, new_chunk);
		}
		polygonize(chunk_idx);
	});
}

void DynamicTerrain::sub_body(ISignedDistanceBody *body) {
	glm::imat2x3 affected = m3d::get_affected_chunks(body->get_bounding_box(), m_chunksize);
	m_chunks.each_inside(affected, [this, body](glm::ivec3 chunk_idx, ISignedDistanceBody *sdf) {
		if (sdf) {
			ISignedDistanceBody *previous_chunk = m_chunks.pop(chunk_idx);
			ISignedDistanceBody *new_chunk = new CSGNode(previous_chunk, body, CSGNode::Operator::DIFF);
			m_chunks.set(chunk_idx, new_chunk);
		}
		polygonize(chunk_idx); // TODO: if this didn't produce any vertices we can just delete the whole chunk?
	});
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
	Mesh *mesh = new Mesh(vertices, false);

	m_chunkmeshes.set(chunk_idx, mesh);

}