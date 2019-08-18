#include <ecs/systems/BillboardRender.hpp>

namespace BillboardRender {
	
	void draw(entt::registry &registry, Billboard &billboard,
		glm::mat4 &uView, glm::mat4 &uProjection) {
		
		registry.view<CPosition, CBillboard>().each(
			[&billboard, &uView, &uProjection](auto entity, auto &pos, auto &bb) {
			
			billboard.draw(uView, uProjection, pos.pos, bb.color, bb.size);
		});
	}
}

