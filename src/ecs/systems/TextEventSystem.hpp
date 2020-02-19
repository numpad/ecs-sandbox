#pragma once

#include <memory>
#include <string>


#include <GL/gl3w.h>
#include <entt/entt.hpp>
#include <glm/glm.hpp>

#include <ecs/systems/BaseUpdateSystem.hpp>
#include <ecs/systems/BaseRenderSystem.hpp>
#include <ecs/events.hpp>
#include <ecs/components.hpp>

#include <RenderObject/Camera.hpp>

#include <Util/Font.hpp>

class TextEventSystem : public BaseUpdateSystem, public BaseRenderSystem {
public:
	
	TextEventSystem(entt::registry &registry, std::shared_ptr<Camera> camera);
	~TextEventSystem();
	
	void receive(const WorldTextEvent &e);
	
	void update();
	void draw();
	
private:
	std::vector<WorldTextEvent> textEvents;
	Font textfont;
};
