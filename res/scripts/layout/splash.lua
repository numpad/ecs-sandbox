local Yoga = require('Yoga')

--[[
+-------------------+
|                   |
|                   |
|      L O G O      |
|                   |
|          helptext |
+-------------------+
]]

return Yoga.parse({
	justifycontent = 'center',
	alignitems = 'center',

	logo = {
		minwidth    =  118 * 3,
		minheight   =   36 * 3,
		width       =    '35%',
		maxwidth    =  118 * 6,
		maxheight   =   36 * 6,
		aspectratio = 118 / 36,
	},
	
	profile = {
		width    = 70,
		height   = 70,
		margin   = 12,
		left     = 0,
		bottom   = 0,
		position = 'absolute',
	},
})
