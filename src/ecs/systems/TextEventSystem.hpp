#pragma once

#include <memory>
#include <string>

#include <GL/gl3w.h>
#include <entt/entt.hpp>
#include <glm/glm.hpp>

#include <ecs/systems/IUpdateSystem.hpp>
#include <ecs/systems/IRenderSystem.hpp>
#include <ecs/events.hpp>
#include <ecs/components.hpp>

#include <RenderObject/Camera.hpp>

#include <Util/Font.hpp>

using namespace glm;

class TextEventSystem : public IUpdateSystem, public IRenderSystem {
public:
	TextEventSystem(entt::registry& registry, std::shared_ptr<Camera> camera);
	~TextEventSystem();

	void receive(const WorldTextEvent& e);

	void update(float dt);
	void draw();

private:
	struct TextEventEntry {
		enum class Animation {
			ZOOM,
			LETTERS
		} animation = Animation::LETTERS;

		WorldTextEvent event;
		vec2 size;

		// zoom
		float scale = 0.0f;
		// letters
		std::wstring displayString;
		int letter_duration = 6;
		int letter_timer = letter_duration;

		TextEventEntry(WorldTextEvent te, vec2 size) : event(te), size(size), displayString(te.text) {
			if (animation == Animation::LETTERS)
				event.text.clear();
		}
	};

	std::vector<TextEventEntry> textEvents;
	Font textfont;
};
