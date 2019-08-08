#pragma once

#include <vector>
#include <assert.h>

template <typename T>
class Grid3D {
	
public:
	
	Grid3D(unsigned int size) : Grid3D(size, size, size) {}
	
	Grid3D(unsigned int x, unsigned int y, unsigned int z)
		: width(x), height(y), depth(z), cells(x * y * z)
	{
		//for (size_t i = 0; i < cells.size(); ++i) cells[i] = new T();
	}
	
	~Grid3D() {
		for (size_t i = 0; i < cells.size(); ++i)
			delete cells[i];
	}
	
	T* at(unsigned int x, unsigned int y, unsigned int z) {
		assert(x < this->width);
		assert(y < this->height);
		assert(z < this->depth);
		
		size_t index = x + y * this->width + z * this->width * this->height;
		return this->at(index);
	}
	
	T* at(unsigned int index) {
		assert(index < this->width * this->height * this->depth);
		
		if (this->cells.at(index) == nullptr)
			this->cells.at(index) = new T();
		return this->cells.at(index);
	}
	
	size_t getCellCount() {
		return this->cells.size();
	}
	
private:
	
	unsigned int width, height, depth;
	std::vector<T*> cells;
	
};
