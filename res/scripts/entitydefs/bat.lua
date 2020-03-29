
entity = {
	name = 'Bat',
	components = {
		CPosition  = vec3.new(),
		CVelocity  = vec3.new(),
		CGravity   = {},
		CHealth    = {hp = 4},
		CJumpTimer = {},
		CBillboard = {
			size = vec2.new(0.2),
			tile = vec2.new(4, 9),
			texture = "res/images/textures/dungeon.png"
		}
	}
}

pos = vec3.new(3, 5, 1)
norm = pos:normalize()
print(pos.x .. ', ' .. pos.y .. ', ' .. pos.z .. ' = ' .. pos:length())
print(norm.x .. ', ' .. norm.y .. ', ' .. norm.z .. ' = ' .. norm:length())
print(pos:dot(norm))
