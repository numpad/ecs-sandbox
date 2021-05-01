#include "ecs/systems/DamageSystem.hpp"

DamageSystem::DamageSystem(entt::registry &registry)
 : IUpdateSystem{registry}
{

}

DamageSystem::~DamageSystem() {

}

void DamageSystem::update(float dt) {
	registry.view<CHealth, CDamageOverTime>().each([this, dt](auto entity, auto &health, auto &dot) {
		// time step
		dot._elapsed_since_beginning += dt;
		dot._elapsed_since_tick += dt;

		while (dot._elapsed_since_tick >= dot.frequency) {
			dot._elapsed_since_tick -= dot.frequency;
			health.hp -= dot.damage_per_tick;
			
			// TODO: fire DamageEvent?
		}
		
		// remove once time is over
		if (dot._elapsed_since_beginning >= dot.duration) {
			registry.remove<CDamageOverTime>(entity);
		}

		// despawn the entity if dead
		if (health.hp <= 0) {
			CPosition *has_position = registry.try_get<CPosition>(entity);
			CExplosive *is_explosive = registry.try_get<CExplosive>(entity);
			if (is_explosive && has_position) {
				registry.ctx<entt::dispatcher>().enqueue<ExplosionEvent>(has_position->pos, is_explosive->radius);
			}

			registry.ctx<entt::dispatcher>().trigger<KillEntityEvent>(entity, "Killed by a wound.");
		}
	});
}
