#include <Assets/TiledTexture.hpp>

TiledTexture::TiledTexture(int tile_width, int tile_height, Texture::Flags flags)
    : Texture(flags), tile_width(tile_width), tile_height(tile_height) {
}

TiledTexture::~TiledTexture() {
}

void TiledTexture::setTile(int x, int y) {
	tile_x = x;
	tile_y = y;

	// tex size
	const float texw = float(getWidth());
	const float texh = float(getHeight());
	// tile size
	const float w = float(tile_width);
	const float h = float(tile_height);

	// pixel to texcoords
	float nx = float(x * tile_width) / texw;
	float ny = (texh - float(y * tile_height) - h) / texh;
	float nw = w / texw;
	float nh = h / texh;

	subrect = glm::vec4(nx, ny, nw, nh);
}
