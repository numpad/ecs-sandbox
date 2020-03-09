#pragma once

#include <unordered_map>
#include <glm/glm.hpp>
#include <functional>

template <class T>
class Grid2D {
	
public:
	
	Grid2D() = default;
	//Grid2D(const Grid2D &copy) = delete;
	~Grid2D() = default;
	
	T *at(int x, int y) const {
		
		auto res = cells.find(std::make_pair(x, y));
		if (res == cells.end()) return nullptr;
		
		return res->second;
	}

	void set(int x, int y, T *value) {
		cells.insert_or_assign(std::make_pair(x, y), value);
	}
	
	T *remove(int x, int y) {
		auto res = cells.find(std::make_pair(x, y));
		if (res == cells.end()) return nullptr;
		
		cells.erase(res);
		return res->second;
	}

	T *at(glm::ivec2 pos) const { return this->at(pos.x, pos.y); }
	void set(glm::ivec2 pos, T *value) { this->set(pos.x, pos.y, value); }
	T *remove(glm::ivec2 pos) { return this->remove(pos.x, pos.y); }
	
	void each(std::function<void (int, int, T *)> func) {
		for (auto iter : cells)
			func(std::get<0>(iter.first), std::get<1>(iter.first), iter.second);
	}
	
	void clear() {
		cells.clear();
	}
	
private:
	
	struct pair_hash {
		template <class T1, class T2>
		size_t operator()(const std::pair<T1, T2> &p) const {
			auto h1 = std::hash<T1>{}(p.first);
			auto h2 = std::hash<T2>{}(p.second);
			return h1 ^ h2; // TODO: improve
		}
	};
	
	int width = 0, height = 0;
	std::unordered_map<std::pair<int, int>, T *, pair_hash> cells;
	
};
