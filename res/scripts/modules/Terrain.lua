local ffi = require('ffi')
ffi.cdef[[
	typedef struct SignedDistTerrain SignedDistTerrain;
	typedef struct ChunkedWorld ChunkedWorld;
	typedef struct { float x, y, z; } vec3;
	
	SignedDistTerrain* FFI_signeddistterrain_new();
	void FFI_signeddistterrain_plane(SignedDistTerrain *terrain, float h);
	void FFI_signeddistterrain_sphere(SignedDistTerrain *terrain, vec3 pos, float r, bool add_or_sub);
	void FFI_signeddistterrain_box(SignedDistTerrain *terrain, vec3 pos, vec3 size, bool add_or_sub);
	void FFI_signeddistterrain_cylinder(SignedDistTerrain *terrain, vec3 pos, float h, float r, bool add_or_sub);
	void FFI_chunkedworld_set(ChunkedWorld *world, void *grid, int x, int y, SignedDistTerrain *terrain);
]]

local Vec3 = require('Vec3')

local Terrain = {}
Terrain.__index = Terrain

--- Creates a new terrain chunk at the given position.
-- @param x (int) The x-position in the world.
-- @param y (int) The y-position in the world.
function Terrain.new(x, y)
	local self = { super = ffi.C.FFI_signeddistterrain_new() }
	ffi.C.FFI_chunkedworld_set(_chunkedWorld, _tileGrid, x, y, self.super)
	return setmetatable(self, Terrain)
end

--- Adds a floor to the terrain, everything below is solid.
-- @param h (number) The height of the floor.
function Terrain.plane(self, h)
	ffi.C.FFI_signeddistterrain_plane(self.super, h)
end

--- Adds or subtracts a sphere to/from the terrain.
-- @param pos (vec3) The position inside the chunk.
-- @param r (float) The spheres radius.
-- @param add_or_sub (bool) Decides if the sphere should be added to or subtracted from the terrain.
function Terrain.sphere(self, pos, r, add_or_sub)
	add_or_sub = add_or_sub ~= false -- optional, defaults to true.
	ffi.C.FFI_signeddistterrain_sphere(self.super, pos, r, add_or_sub)
end

--- Adds or subtracts a box to/from the terrain.
-- @param pos (vec3) The position inside the chunk.
-- @param size (vec3) The size of the box.
-- @param add_or_sub (bool) Decides if the box should be added to or subtracted from the terrain.
function Terrain.box(self, pos, size, add_or_sub)
	add_or_sub = add_or_sub ~= false -- optional, defaults to true.
	ffi.C.FFI_signeddistterrain_box(self.super, pos, size, add_or_sub)
end

--- Adds or subtracts a cylinder to/from the terrain.
-- The cylinder is a circle on the xz-plane with height h.
-- @param pos (vec3) The position inside the chunk.
-- @param h (float) The height of the cylinder.
-- @param r (float) The radius of the cylinder.
-- @param add_or_sub (bool) Decides if the cylinder should be added to or subtracted from the terrain.
function Terrain.cylinder(self, pos, h, r, add_or_sub)
	add_or_sub = add_or_sub ~= false -- optional, defaults to true.
	ffi.C.FFI_signeddistterrain_cylinder(self.super, pos, r, h, add_or_sub)
end

return Terrain
