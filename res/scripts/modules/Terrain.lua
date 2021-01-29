local ffi = require('ffi')
ffi.cdef[[
	typedef struct SignedDistTerrain SignedDistTerrain;
	typedef struct ChunkedWorld ChunkedWorld;
	
	SignedDistTerrain* FFI_signeddistterrain_new();
	void FFI_signeddistterrain_plane(SignedDistTerrain *terrain, float h);
	void FFI_signeddistterrain_sphere(SignedDistTerrain *terrain, float x, float y, float z, float r, bool add_or_sub);
	void FFI_signeddistterrain_box(SignedDistTerrain *terrain, float x, float y, float z, float a, float b, float c, bool add_or_sub);
	void FFI_chunkedworld_set(ChunkedWorld *world, void *grid, int x, int y, SignedDistTerrain *terrain);
]]

local Terrain = {}
Terrain.__index = Terrain

function Terrain.new(x, y)
	local self = { super = ffi.C.FFI_signeddistterrain_new() }
	ffi.C.FFI_chunkedworld_set(_chunkedWorld, _tileGrid, x, y, self.super)
	return setmetatable(self, Terrain)
end

function Terrain.plane(self, h)
	ffi.C.FFI_signeddistterrain_plane(self.super, h)
end

function Terrain.sphere(self, x, y, z, r, add_or_sub)
	add_or_sub = add_or_sub ~= false -- optional, defaults to true.
	ffi.C.FFI_signeddistterrain_sphere(self.super, x, y, z, r, add_or_sub)
end

function Terrain.box(self, x, y, z, a, b, c, add_or_sub)
	add_or_sub = add_or_sub ~= false -- optional, defaults to true.
	ffi.C.FFI_signeddistterrain_box(self.super, x, y, z, a, b, c, add_or_sub)
end

return Terrain
