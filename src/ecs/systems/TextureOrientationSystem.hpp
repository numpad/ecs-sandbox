#pragma once

#include <memory>

#include <glm/glm.hpp>
#include <entt/entt.hpp>

#include <ecs/systems/IUpdateSystem.hpp>
#include <ecs/components.hpp>
#include <RenderObject/Camera.hpp>

class TextureOrientationSystem : public IUpdateSystem {
public:

	TextureOrientationSystem(entt::registry &registry, std::shared_ptr<Camera> camera);
	~TextureOrientationSystem();

	void update(float dt) override;

private:
	std::shared_ptr<Camera> m_camera;
};
