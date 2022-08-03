local Yoga = require('Yoga')

local layout = Yoga.parse({
	flexdirection = "column",
	maincontainer = {
		flexdirection = "column-reverse",
		justifycontent = "center",
		maxwidth = '50%',
		height = '100%',
		padding = 20,

		logo = {
			width = 400,
			height = 130,
		},
		container = {
			flexdirection = "column",
			width = 280,

			continue = {
				width = '100%',
				height = 40,
			},
			startgame = {
				width = '100%',
				height = 40,
			},
			settings = {
				width = '100%',
				height = 40,
			},
			quit = {
				width = '100%',
				height = 40,
			},
		},
	},

})

return layout

