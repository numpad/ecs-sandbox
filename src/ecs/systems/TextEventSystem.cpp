#include <ecs/systems/TextEventSystem.hpp>

TextEventSystem::TextEventSystem(entt::registry &registry, std::shared_ptr<Camera> camera)
	: IUpdateSystem(registry), IRenderSystem(registry, camera),
	textfont("res/fonts/humble_fonts/futile-pro-v1/FutilePro.ttf", 42) {
	
	registry.ctx<entt::dispatcher>().sink<WorldTextEvent>().connect<&TextEventSystem::receive>(*this);
}

TextEventSystem::~TextEventSystem() {
	registry.ctx<entt::dispatcher>().sink<WorldTextEvent>().disconnect<&TextEventSystem::receive>(*this);
	textfont.destroy();
}


void TextEventSystem::receive(const WorldTextEvent &e) {
	textEvents.push_back(TextEventEntry(e, textfont.getSize(e.text)));
}


void TextEventSystem::update(float dt) {
	for (auto &te : textEvents) {
		switch (te.animation) {
			case TextEventEntry::Animation::ZOOM: {
				if (te.event.duration > 0) {
					if (te.scale >= 1.0f) {
						te.event.duration--;
					} else {
						te.scale += 0.02f;
						te.scale *= 1.7f;
						if (te.scale > 1.0f) te.scale = 1.0f;
					}
				} else {
					te.scale *= 0.8f;
					if (te.scale < 0.05f) te.event.duration = -1;
				}
				break;
			}
			case TextEventEntry::Animation::LETTERS: {
				te.scale = 1.f;
				
				if (te.event.text.size() < te.displayString.size()) {
					if (te.letter_timer-- <= 0) {
						te.letter_timer = te.letter_duration;
						te.event.text += te.displayString[te.event.text.size()];
						te.size = textfont.getSize(te.event.text);
					}
				} else {
					te.event.duration--;
				}
				break;
			}
		};
	}
	
	textEvents.erase(std::remove_if(textEvents.begin(), textEvents.end(),
		[](auto &e) {
			return e.event.duration < 0;
		}), textEvents.end());
}

void TextEventSystem::draw() {
	for (auto &te : textEvents) {
		vec3 pos = vec3(0.f);
		if (registry.valid(te.event.who) && registry.has<CPosition>(te.event.who))
			pos = registry.get<CPosition>(te.event.who).pos;
		pos += te.event.offset;
		
		vec2 wtos = camera->worldToScreen(pos) - te.size * te.scale * .5f;
		textfont.drawString(camera->getHudProjection(), te.event.text, wtos.x, wtos.y,
			te.scale, vec3(.82f, .29f, .25f));
		
	}
}
