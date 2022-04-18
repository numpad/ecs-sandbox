-- init.lua - Executed when the engine is initialized

-- Abort initialization by calling error()
-- error('Something bad happened, aborting...')

-- TODO: check how to load layouts in the best way
local splashlayout_require = require('layout.splash')
--local splashlayout_dofile = dofile('res/scripts/layout/splash.lua')
--assert(splashlayout_require:getchildcount() == splashlayout_dofile:getchildcount())

splashlayout_require:calculate()
print('root: ' .. splashlayout_require:get_layoutleft())
print('logo: ' .. splashlayout_require:getchild(1):get_layoutleft())
