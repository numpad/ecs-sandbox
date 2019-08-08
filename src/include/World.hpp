#pragma once

#include <stdio.h>
#include <Grid3D.hpp>

class World {
	
public:
	World();
	
	void draw();
	
private:
	unsigned int width, height, depth;
	Grid3D<int> grid;
	
};
