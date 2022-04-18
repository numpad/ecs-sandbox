local Yoga = require('Yoga')

return Yoga.layout({
	alignitems = 'flex-start',
	
	-- children:
	menu = {
		width = '50%',
		justifycontent = 'center',
		alignitems = 'center',
		margin = 0,
		flexgrow = 1,

		-- children:
		logo = {
			width = '80%',
			height = 80,
		},
		buttons = {
			width = '80%',
			play = {
				width = 100,
				height = 20,
			},
			options = {
				width = 100,
				height = 20,
			},
		},
	}
})
