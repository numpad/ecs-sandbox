local ffi = require('ffi')
ffi.cdef[[
	typedef struct SignedDistTerrain SignedDistTerrain;
	typedef struct ChunkedWorld ChunkedWorld;
	typedef struct { float x, y, z; } vec3;
	
	SignedDistTerrain* FFI_signeddistterrain_new();
	void FFI_signeddistterrain_plane(SignedDistTerrain *terrain, float h);
	void FFI_signeddistterrain_sphere(SignedDistTerrain *terrain, vec3 pos, float r, bool add_or_sub);
	void FFI_signeddistterrain_box(SignedDistTerrain *terrain, vec3 pos, vec3 size, bool add_or_sub);
	void FFI_chunkedworld_set(ChunkedWorld *world, void *grid, int x, int y, SignedDistTerrain *terrain);
]]

local Vec3 = require('Vec3')

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

function Terrain.sphere(self, pos, r, add_or_sub)
	add_or_sub = add_or_sub ~= false -- optional, defaults to true.
	ffi.C.FFI_signeddistterrain_sphere(self.super, pos, r, add_or_sub)
end

function Terrain.box(self, pos, size, add_or_sub)
	add_or_sub = add_or_sub ~= false -- optional, defaults to true.
	ffi.C.FFI_signeddistterrain_box(self.super, pos, size, add_or_sub)
end

return Terrain
