
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


-- testing components
p = CPosition.new(3, 2, 1)
print(p.pos.x .. '_' .. p.pos.y .. '_' .. p.pos.z)


