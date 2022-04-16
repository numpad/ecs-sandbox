
print('\n   ###\n   ### Hello, World!\n   ###\n')

function update()
	while true do
		print("* update from lua!")
		coroutine.yield()
	end
end
