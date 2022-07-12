-- init.lua - Executed when the engine is initialized

-- Abort initialization by calling error()
-- error('Something bad happened, aborting...')

local Engine = require('Engine');

local scene = Engine:load_scene('SplashScreenScene')
Engine:set_active_scene(scene)
