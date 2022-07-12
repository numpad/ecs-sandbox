#pragma once

#include <memory>

#include <entt/entt.hpp>

#include <RenderObject/Camera.hpp>

class IRenderSystem {
public:
	IRenderSystem(const entt::registry& cregistry, std::shared_ptr<Camera> camera)
	    : cregistry(cregistry), camera(camera) {
	}

	virtual ~IRenderSystem() {
	}
	virtual void draw() = 0;

	// setters
	inline void setCamera(std::shared_ptr<Camera> camera) {
		this->camera = camera;
	}

protected:
	const entt::registry& cregistry;
	std::shared_ptr<Camera> camera;
};
