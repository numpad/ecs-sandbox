local ffi = require('ffi')
ffi.cdef[[
	typedef struct SignedDistTerrain SignedDistTerrain;
	typedef struct ChunkedWorld ChunkedWorld;
	typedef struct ChunkedTerrain ChunkedTerrain;
	typedef struct { float x, y, z; } vec3;

	SignedDistTerrain* FFI_signeddistterrain_new();
	void FFI_signeddistterrain_plane(SignedDistTerrain *terrain, vec3 pos, vec3 normal, float h);
	void FFI_signeddistterrain_sphere(SignedDistTerrain *terrain, vec3 pos, float r, bool add_or_sub);
	void FFI_signeddistterrain_box(SignedDistTerrain *terrain, vec3 pos, vec3 size, bool add_or_sub);
	void FFI_signeddistterrain_cylinder(SignedDistTerrain *terrain, vec3 pos, float h, float r, bool add_or_sub);
	void FFI_chunkedworld_set(ChunkedWorld *world, void *grid, int x, int y, SignedDistTerrain *terrain);
	float FFI_chunkedTerrain_raycast(ChunkedTerrain *terrain, vec3 p, vec3 d, float max_len);
	float FFI_chunkedTerrain_sampleValue(ChunkedTerrain *terrain, vec3 p);
]]

local Vec3 = require('Vec3')

local Terrain = {}
Terrain.__index = Terrain

--- Casts a ray and tests for intersection with the terrain.
-- @param origin (vec3) The ray origin.
-- @param dir (vec3) The ray direction.
-- @param max_length (number) The maximum distance to travel.
-- @return The distance until an intersection occured, or -1 if no intersection occured.
function Terrain.raycast(origin, dir, max_length)
	return ffi.C.FFI_chunkedTerrain_raycast(_chunkedTerrain, origin, dir, max_length)
end

--- Evaluates the terrains signed distance function and returns its result.
-- @param point (vec3) The point for which the SDF should be calculated.
-- @return The distance between `point` and the nearest point inside the terrain.
function Terrain.sampleValue(point)
	return ffi.C.FFI_chunkedTerrain_sampleValue(_chunkedTerrain, point)
end

--- Creates a new terrain chunk at the given position.
-- @param x (int) The x-position in the world.
-- @param y (int) The y-position in the world.
-- @return A newly created terrain object.
function Terrain.new(x, y)
	local self = { super = ffi.C.FFI_signeddistterrain_new() }
	ffi.C.FFI_chunkedworld_set(_chunkedWorld, _tileGrid, x, y, self.super)
	return setmetatable(self, Terrain)
end

--- Adds a plane to the terrain, everything below is solid.
-- @param p (vec3) The position of the plane.
-- @param n (vec3) The upwards facing normal of the plane.
-- @param h (number) The height of the floor.
-- @return The terrain object itself for chaining operations.
function Terrain:plane(p, n, h)
	ffi.C.FFI_signeddistterrain_plane(self.super, p, n, h)
	return self
end

--- Adds a floor to the terrain, everything below is solid.
-- @param h (number) The height of the floor.
-- @return The terrain object itself for chaining operations.
function Terrain:floor(h)
	ffi.C.FFI_signeddistterrain_plane(self.super, Vec3(0, 0, 0), Vec3(0, 1, 0), h)
	return self
end

--- Adds or subtracts a sphere to/from the terrain.
-- @param pos (vec3) The position inside the chunk.
-- @param r (float) The spheres radius.
-- @param add_or_sub (bool) Decides if the sphere should be added to or subtracted from the terrain.
-- @return The terrain object itself for chaining operations.
function Terrain:sphere(pos, r, add_or_sub)
	add_or_sub = add_or_sub ~= false -- optional, defaults to true.
	ffi.C.FFI_signeddistterrain_sphere(self.super, pos, r, add_or_sub)
	return self
end

--- Adds or subtracts a box to/from the terrain.
-- @param pos (vec3) The position inside the chunk.
-- @param size (vec3) The size of the box.
-- @param add_or_sub (bool) Decides if the box should be added to or subtracted from the terrain.
-- @return The terrain object itself for chaining operations.
function Terrain:box(pos, size, add_or_sub)
	add_or_sub = add_or_sub ~= false -- optional, defaults to true.
	ffi.C.FFI_signeddistterrain_box(self.super, pos, size, add_or_sub)
	return self
end

--- Adds or subtracts a cylinder to/from the terrain.
-- The cylinder is a circle on the xz-plane with height h.
-- @param pos (vec3) The position inside the chunk.
-- @param h (float) The height of the cylinder.
-- @param r (float) The radius of the cylinder.
-- @param add_or_sub (bool) Decides if the cylinder should be added to or subtracted from the terrain.
-- @return The terrain object itself for chaining operations.
function Terrain:cylinder(pos, h, r, add_or_sub)
	add_or_sub = add_or_sub ~= false -- optional, defaults to true.
	ffi.C.FFI_signeddistterrain_cylinder(self.super, pos, r, h, add_or_sub)
	return self
end

return Terrain
