local Yoga = require('Yoga')

local layout = Yoga.parse({
	justifycontent = 'center',
	alignitems     = 'flex-start',

	{
		"container",
		width = 280,
		minheight = 300,
	},
	{
		"logo",
		width = 400,
		height = 130,
	},

})

return layout

