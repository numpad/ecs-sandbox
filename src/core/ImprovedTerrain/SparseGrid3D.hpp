#pragma once

#include <unordered_map>
#include <functional>

#include <glm/glm.hpp>

#include <ImprovedTerrain/ISignedDistanceFunction.hpp>

template <typename T>
class SparseGrid3D {
public:
	T* at(const glm::ivec3& p) const {
		auto search = m_grid.find(p);
		if (search == m_grid.end())
			return nullptr;

		return search->second;
	}

	void set(const glm::ivec3& p, T* element) {
		m_grid[p] = element;
	}

	T* pop(const glm::ivec3& p) {
		auto found = at(p);
		if (found != nullptr)
			set(p, nullptr);
		return found;
	}

	void each(std::function<void(glm::ivec3, T*)> func) {
		for (auto iter : m_grid)
			func(iter.first, iter.second);
	}

	void each_inside(glm::imat2x3 chunks_aabb, std::function<void(glm::ivec3, T*)> func) {
		const glm::ivec3 min = chunks_aabb[0];
		const glm::ivec3 max = chunks_aabb[1];

		for (int x = min.x; x <= max.x; ++x) {
			for (int y = min.y; y <= max.y; ++y) {
				for (int z = min.z; z <= max.z; ++z) {
					func(glm::ivec3(x, y, z), at(glm::ivec3(x, y, z)));
				}
			}
		}
	}

private:
	struct ivec3_hash {
		size_t operator()(const glm::ivec3& v) const {
			return std::hash<int>{}(v.x) ^ std::hash<int>{}(v.y) ^ std::hash<int>{}(v.z);
		}
	};
	std::unordered_map<glm::ivec3, T*, ivec3_hash> m_grid;
};
