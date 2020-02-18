#pragma once

#include <entt/entt.hpp>

#include <RenderObject/Camera.hpp>

class BaseRenderSystem {
public:
	
	BaseRenderSystem(const entt::registry &cregistry, const Camera &camera)
		: cregistry(cregistry), camera(camera) {}
	
	virtual ~BaseRenderSystem() { }
	virtual void draw() = 0;

protected:
	const entt::registry &cregistry;
	const Camera &camera;
	
};
