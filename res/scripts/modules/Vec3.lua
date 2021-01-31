local ffi = require('ffi')
ffi.cdef[[
	typedef struct { float x, y, z; } vec3;

	vec3 FFI_vec3_add(vec3 a, vec3 b);
	vec3 FFI_vec3_sub(vec3 a, vec3 b);
	vec3 FFI_vec3_mul(vec3 a, vec3 b);
	vec3 FFI_vec3_smul(vec3 a, float s);
	vec3 FFI_vec3_div(vec3 a, vec3 b);
	vec3 FFI_vec3_sdiv(vec3 a, float s);
	float FFI_vec3_dot(vec3 a, vec3 s);
	vec3 FFI_vec3_cross(vec3 a, vec3 b);
	float FFI_vec3_len(vec3 a);
	vec3 FFI_vec3_normalize(vec3 a);
]]

local Vec3
local glm_vec3 = {
	__add = ffi.C.FFI_vec3_add,
	__sub = ffi.C.FFI_vec3_sub,
	__mul = function (a, b)
		if type(b) == 'number' then return ffi.C.FFI_vec3_smul(a, b) end
		return ffi.C.FFI_vec3_mul(a, b)
	end,
	__div = function (a, b)
		if type(b) == 'number' then return ffi.C.FFI_vec3_sdiv(a, b) end
		return ffi.C.FFI_vec3_div(a, b)
	end,
	__len = function (a) return ffi.C.FFI_vec3_len(a) end,
	__index = {
		length = ffi.C.FFI_vec3_len,
		dot = ffi.C.FFI_vec3_dot,
		cross = ffi.C.FFI_vec3_cross,
		normal = ffi.C.FFI_vec3_normalize
	}
}

-- https://luajit.org/ext_ffi_tutorial.html
Vec3 = ffi.metatype("vec3", glm_vec3)

return Vec3
