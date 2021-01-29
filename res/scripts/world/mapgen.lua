local Terrain = require('Terrain')
local Vec3 = require('Vec3')

-- run mapgen
for i = 0, 3 do
	local t = Terrain.new(i / 2, i % 2)
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
