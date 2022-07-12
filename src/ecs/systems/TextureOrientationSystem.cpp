#include <ecs/systems/TextureOrientationSystem.hpp>

TextureOrientationSystem::TextureOrientationSystem(entt::registry& registry, std::shared_ptr<Camera> camera)
    : IUpdateSystem{registry}, m_camera{camera} {
}

TextureOrientationSystem::~TextureOrientationSystem() {
}

void TextureOrientationSystem::update(float dt) {
	registry.view<COrientedTexture, CTextureRegion>().each([](const auto entity, auto orientation, auto region) {

	});
}
