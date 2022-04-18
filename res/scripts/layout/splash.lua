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

return Yoga.layout({
	width = 500,
	height = 500,
	justifycontent = 'center',
	alignitems = 'center',
	
	logo = {
		width = '70%',
		height = 130,
		margin = 'auto',
	},
	
	helptext = {
		width = 200,
		height = 60,
		margin = 10,
		right = 0,
		bottom = 0,
		position = 'absolute',
	},
})
