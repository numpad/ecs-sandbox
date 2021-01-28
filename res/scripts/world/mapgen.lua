local Terrain = require('Terrain')
local Vec3 = require('Vec3')

--- Creates a room with 4 doors at each side.
function Room4Doors(x, y)
	local t = Terrain.new(x, y)
	t:floor(0.7)
	t:box(Vec3(0, 0.5, 0), Vec3(1.75, 0.5, 1.75), false)
	
	-- doors
	t:sphere(Vec3( 0, 0,  2), 0.5, false)
	t:sphere(Vec3( 0, 0, -2), 0.5, false)
	t:sphere(Vec3(-2, 0,  0), 0.5, false)
	t:sphere(Vec3( 2, 0,  0), 0.5, false)

	-- fix all holes
	t:floor(0)
end

-- run mapgen
Room4Doors(0, 0)
Room4Doors(0, 1)

-- downward slopes
Terrain.new(1, 0)
	:plane(Vec3(0, 0, 0), Vec3(0.5, 1, 0):normal(), -0.9)
	:floor(-1)

Terrain.new(1, 1)
	:plane(Vec3(0, 0, 0), Vec3(0.5, 1, 0):normal(), -0.9)
	:floor(-0.5)
	:box(Vec3(0.5, -1, -1.5), Vec3(0.4, 0.3, 0.5), false)
	:floor(-1)
	:cylinder(Vec3(-0.5, -1, 0), 1, 0.5, false)
