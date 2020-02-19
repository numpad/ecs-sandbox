#include <ecs/systems/TextEventSystem.hpp>

TextEventSystem::TextEventSystem(entt::registry &registry, std::shared_ptr<Camera> camera)
	: BaseUpdateSystem(registry), BaseRenderSystem(registry, camera),
	textfont("res/fonts/FFF_Tusj.ttf", 32) {
	
	registry.ctx<entt::dispatcher>().sink<WorldTextEvent>().connect<&TextEventSystem::receive>(*this);
}

TextEventSystem::~TextEventSystem() {
	// does this make sense?
	// registry.ctx<entt::dispatcher>().sink<WorldTextEvent>().disconnect<&TextEventSystem::receive>(*this);
	textfont.destroy();
}


void TextEventSystem::receive(const WorldTextEvent &e) {
	textEvents.push_back(e);
}


void TextEventSystem::update() {
	for (auto &te : textEvents) {
		te.duration--;
	}
	
	textEvents.erase(std::remove_if(textEvents.begin(), textEvents.end(),
		[](auto &e) {
			return e.duration <= 0;
		}), textEvents.end());
}

void TextEventSystem::draw() {
	for (auto &te : textEvents) {
		vec3 pos = vec3(0.f);
		if (registry.valid(te.who) && registry.has<CPosition>(te.who))
			pos = registry.get<CPosition>(te.who).pos;
		pos += te.offset;
		
		vec2 wtos = camera->worldToScreen(pos);
		textfont.drawString(camera->getHudProjection(), te.text, wtos.x, wtos.y, 1.f, vec3(1.f, 1.f, 1.f));
	}
}
