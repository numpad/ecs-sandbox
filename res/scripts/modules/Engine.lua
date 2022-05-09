local ffi = require('ffi')
ffi.cdef[[
	void FFI_Engine_setActiveScene(void *engine, void *scene);
	void* FFI_Scene_TowerScene();
	void* FFI_Scene_MainMenuScene();
	void* FFI_Scene_SplashScreenScene();
]]

local Engine = { instance = _Engine }
Engine.__index = Engine

function Engine:set_active_scene(scene)
	ffi.C.FFI_Engine_setActiveScene(self.instance, scene)
end

function Engine:load_scene(name)
	if name == 'TowerScene' then
		return ffi.C.FFI_Scene_TowerScene()
	elseif name == 'MainMenuScene' then
		return ffi.C.FFI_Scene_MainMenuScene()
	elseif name == 'SplashScreenScene' then
		return ffi.C.FFI_Scene_SplashScreenScene()
	end

	return nil
end

return Engine
