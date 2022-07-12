#pragma once

#include <memory>
#include <glm/glm.hpp>
#include <ecs/systems/IUpdateSystem.hpp>
#include <ecs/components.hpp>

#include <ImprovedTerrain/ISignedDistanceFunction.hpp>
#include <ImprovedTerrain/ISignedDistanceBody.hpp>
#include <Util/Math3d.hpp>

class DistanceFunctionCollisionSystem : public IUpdateSystem {
public:
	DistanceFunctionCollisionSystem(entt::registry& registry, ISignedDistanceFunction& m_sdf);
	~DistanceFunctionCollisionSystem();

	void update(float dt) override;

private:
	ISignedDistanceFunction& m_sdf;
};
