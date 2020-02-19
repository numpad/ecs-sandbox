#pragma once

#include <memory>

#include <entt/entt.hpp>

#include <RenderObject/Camera.hpp>

class BaseRenderSystem {
public:
	
	BaseRenderSystem(const entt::registry &cregistry, std::shared_ptr<Camera> camera)
		: cregistry(cregistry), camera(camera) {}
	
	virtual ~BaseRenderSystem() { }
	virtual void draw() = 0;
	
	// setters
	inline void setCamera(std::shared_ptr<Camera> camera) { this->camera = camera; }
	
protected:
	const entt::registry &cregistry;
	std::shared_ptr<Camera> camera;
	
};
