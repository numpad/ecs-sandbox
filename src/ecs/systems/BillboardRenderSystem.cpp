#include <ecs/systems/BillboardRenderSystem.hpp>

void BillboardRenderSystem::draw(entt::registry &registry, Billboard &billboard,
	glm::mat4 &uView, glm::mat4 &uProjection) {
	
	static glm::vec3 zero(0.0f);
	glm::vec3 *renderTarget;
	ImGui::Begin("bbRenderSystem");
		static bool checked = false;
		ImGui::Checkbox("Flat billboards?", &checked);
		if (checked) {
			renderTarget = &zero;
		} else {
			renderTarget = nullptr;
		}
	ImGui::End();
	
	
	registry.view<CPosition, CBillboard>().each(
		[&billboard, &uView, &uProjection, renderTarget](auto entity, auto &pos, auto &bb) {
		
		billboard.draw(uView, uProjection, pos.pos, bb.size, renderTarget);
	});
}
