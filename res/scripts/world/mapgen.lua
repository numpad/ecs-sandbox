local Terrain = require('Terrain')
local Vec3 = require('Vec3')

--- Creates a room with 4 doors at each side.
function Room4Doors(x, y)
	local t = Terrain.new(x, y)
	t:plane(0.7)
	t:box(Vec3(0, 0.5, 0), Vec3(1.75, 0.5, 1.75), false)
	
	-- doors
	t:sphere(Vec3( 0, 0,  2), 0.5, false)
	t:sphere(Vec3( 0, 0, -2), 0.5, false)
	t:sphere(Vec3(-2, 0,  0), 0.5, false)
	t:sphere(Vec3( 2, 0,  0), 0.5, false)

	-- fix all holes
	t:plane(0)
end

-- run mapgen
--for i = 0, 3 do
--	Room4Doors(i / 2, i % 2)
--end

local ts = {}
ts[0] = Terrain.new(0, 0)
ts[1] = Terrain.new(0, 1)
ts[2] = Terrain.new(1, 0)
ts[3] = Terrain.new(1, 1)

local dist = 1.75
ts[0]:cylinder(Vec3( dist, -1,  dist), 1, 2)
ts[1]:cylinder(Vec3( dist, -1, -dist), 1, 2)
ts[2]:cylinder(Vec3(-dist, -1,  dist), 1, 2)
ts[3]:cylinder(Vec3(-dist, -1, -dist), 1, 2)

ts[0]:cylinder(Vec3(-dist, -0.5, -dist), 0.5, 1)
