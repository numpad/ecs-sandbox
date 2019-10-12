#pragma once

#include <Assets/Texture.hpp>

class TiledTexture : public Texture {
public:
	
	TiledTexture(int tile_width, int tile_height,
		Texture::Flags flags = Texture::Flags::NONE);
	virtual ~TiledTexture();
	
	// specialized subrect
	void setTile(int x, int y);
	inline void getTile(int &x, int &y) { x = tile_x; y = tile_y; }
	
private:
	int tile_width, tile_height;
	
	int tile_x = 0, tile_y = 0;
};
