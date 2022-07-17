#pragma once

#include "ImprovedTerrain/SparseGrid3D.hpp"
#include "scenes/gamemode/raft/RaftBlock.hpp"
#include <glm/fwd.hpp>
#include <glm/glm.hpp>

class RaftSet {
public:

	RaftSet() {
		m_testblock = new RaftBlock();
		m_testblock->id = 1;

		for (size_t i = 0; i < 4; ++i) {
			m_blocks.set(glm::ivec3(i, 0, i), m_testblock);
		}
	}
	
	~RaftSet() {
		delete m_testblock;
	}


	SparseGrid3D<RaftBlock> m_blocks;

private:
	RaftBlock *m_testblock;
};

