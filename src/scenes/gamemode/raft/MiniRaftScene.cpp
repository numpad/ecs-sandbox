#include "scenes/gamemode/raft/MiniRaftScene.hpp"
#include "Assets/Loaders/ObjLoader.hpp"
#include "Graphics/GLState.hpp"
#include "RenderObject/GBuffer.hpp"
#include "Util/File.hpp"
#include "Util/Math3d.hpp"
#include "Util/Random.hpp"
#include "ecs/components.hpp"
#include "ecs/components/CGravity.hpp"
#include "ecs/components/COrientation.hpp"
#include "ecs/components/CPosition.hpp"
#include "ecs/events/MouseButtonEvent.hpp"
#include "ecs/systems/BillboardRenderSystem.hpp"
#include "ecs/systems/DecalRenderSystem.hpp"
#include "ecs/systems/IRenderSystem.hpp"
#include "ecs/systems/IUpdateSystem.hpp"
#include "ecs/systems/ModelRenderSystem.hpp"
#include <GLFW/glfw3.h>
#include <filesystem>
#include <fmt/core.h>
#include <glm/ext/scalar_constants.hpp>
#include <imgui.h>
#include <luajit/lua.h>
#include <luajit/lauxlib.h>

static glm::vec3 limitMag(glm::vec3 of, glm::vec3 to) {
	const float mof = glm::length(of);
	const float mto = glm::length(to);

	if (mof > mto) {
		return glm::normalize(of) * mto;
	}
	return of;
}

bool MiniRaftScene::onCreate() {
	m_engine->getDispatcher().sink<MouseButtonEvent>().connect<&MiniRaftScene::onMouseButtonInput>(this);
	m_registry.set<entt::dispatcher>();
	
	m_camera = std::make_shared<Camera>(glm::vec3(-4.0f, 4.5f, -4.0f));
	m_camera->setTarget(glm::vec3(0.0f));

	/* update systems */
	BillboardRenderSystem* billboardRenderSystem = new BillboardRenderSystem(m_registry, m_camera);
	m_updatesystems.emplace_back(new GravitySystem(m_registry, 0.000981f));
	m_updatesystems.emplace_back(new PositionUpdateSystem(m_registry));
	m_updatesystems.emplace_back(billboardRenderSystem);
	/* render systems */
	m_rendersystems.emplace_back(new ModelRenderSystem(m_registry, m_camera));
	m_rendersystems.push_back(billboardRenderSystem);
	m_rendersystems.emplace_back(new DecalRenderSystem(m_registry, m_camera)); // needs to be rendered after OceanPlane
	m_rendersystems.emplace_back(new LightVolumeRenderSystem(m_registry, m_camera));

	return true;
}

void MiniRaftScene::onDestroy() {
	m_engine->getDispatcher().sink<MouseButtonEvent>().disconnect<&MiniRaftScene::onMouseButtonInput>(this);

	std::for_each(m_updatesystems.begin(), m_updatesystems.end(), [](IUpdateSystem* usys) {
		delete usys;
	});
	// TODO: double-freeing billboardRenderSystem
	// std::for_each(m_rendersystems.begin(), m_rendersystems.end(), [](IRenderSystem* rsys) { delete rsys; });
}

void MiniRaftScene::onUpdate(float dt) {
#ifndef NDEBUG
	static float _deleteTimer = 0.0f;
	if ((_deleteTimer += dt) > 25.0f) {
		_deleteTimer = 0.0f;
		m_registry.clear();
	}

	static char spawnFunc[1024] = "-- position\npx = math.sin(TIME)\npy = 0\npz = 3\n\n-- velocity\nvx = "
	                              "(math.random() * 2 - 1) * 0.3\nvy = -1\nvz = math.random() * 0.04 + 0.012";

	lua_State* L = Engine::Instance->getLuaState();
	lua_pushnumber(L, glfwGetTime());
	lua_setglobal(L, "TIME");
	luaL_dostring(L, spawnFunc);
	lua_getglobal(L, "vx");
	lua_getglobal(L, "vy");
	lua_getglobal(L, "vz");
	float _x = lua_tonumber(L, -3);
	float _y = lua_tonumber(L, -2);
	float _r = lua_tonumber(L, -1);
	lua_pop(L, 3);
	lua_getglobal(L, "px");
	lua_getglobal(L, "py");
	lua_getglobal(L, "pz");
	float _px = lua_tonumber(L, -3);
	float _py = lua_tonumber(L, -2);
	float _pz = lua_tonumber(L, -1);
	lua_pop(L, 3);

	
	if (!m_registry.valid(m_spawnerBox)) {
		m_spawnerBox = m_registry.create();
	}
	m_registry.emplace_or_replace<CPosition>(m_spawnerBox, _px, _py, _pz);
	m_registry.emplace_or_replace<COrientation>(m_spawnerBox, glm::vec3(1.0f, 0.0f, 0.0f), -0.4f);
	m_registry.emplace_or_replace<CModel>(m_spawnerBox, m_assetmanager.getMesh("res/models/raft/boxOpen.obj"));
	
	static float timer = 1.0f;
	if ((timer += dt) > 0.06f) {
		timer = 0.0f;
		static Random rnd(0.0f, 1.0f);
		const glm::vec2 rdir = glm::normalize(glm::vec2(_x, _y)) * _r;
		Texture* texture = m_assetmanager.getTexture("res/images/textures/dungeon.png");
		entt::entity e = m_registry.create();
		m_registry.emplace<CPosition>(e, glm::vec3(_px, _py, _pz));
		m_registry.emplace<CVelocity>(e, glm::vec3(rdir.x, 0.03f, rdir.y));
		m_registry.emplace<CGravity>(e);

		static int i = 0;
		if (i++ % 2 == 1) {
			m_registry.emplace<CBillboard>(e, texture, glm::vec2(0.2f));
			m_registry.emplace<CTextureRegion>(e, glm::floor(rnd() * 5.0f) * 16.0f,
											   (9.0f + glm::floor(rnd() * 5.0f)) * 16.0f, 16.0f, 16.0f, 256, 256);
		} else {
			glm::vec3 rot = glm::normalize(glm::vec3(rnd(), rnd(), rnd()));
			m_registry.emplace<COrientation>(e, rot, rnd() * glm::pi<float>() * 2.0f);
			if ((i / 2) % 2 == 0)
				m_registry.emplace<CModel>(e, m_assetmanager.getMesh("res/models/raft/barrel.obj"));
			else
				m_registry.emplace<CModel>(e, m_assetmanager.getMesh("res/models/raft/box.obj"));
		}
	}
	
#endif

	std::for_each(m_updatesystems.begin(), m_updatesystems.end(), [dt](auto& usys) {
		usys->update(dt);
	});

	// make entites float
	m_registry.view<const CPosition, CVelocity, const CGravity>().each([](const CPosition& cpos, CVelocity& cvel, const auto& cgravity) {
		const float WATER_HEIGHT = 0.0f - 0.05f; // TODO: something like oceanplane.getHeightAtPos(cpos.pos.xz);
		if (cpos.pos.y < WATER_HEIGHT) {
			const glm::vec3 drag = -limitMag(glm::normalize(cvel.vel) * 0.0008f, cvel.vel);
			const glm::vec3 buoyancy = glm::vec3(0.0f, 0.002f, 0.0f);
			cvel.vel += drag + buoyancy;
		}
	});
}

void MiniRaftScene::onRender() {
	glClearColor(0.1f, 0.0f, 0.24f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	std::for_each(m_rendersystems.begin(), m_rendersystems.end(), [](auto& rsys) {
		rsys->draw();
	});

	// render water
	m_waterplane.draw(*m_camera);

}

/////////////
// PRIVATE //
/////////////

void MiniRaftScene::onMouseButtonInput(const MouseButtonEvent& event) {
	if (!event.is_down) return;

	glm::vec2 mpos = Engine::Instance->getWindow().getNormalizedMousePosition();
	m3d::ray ray = m_camera->raycast(mpos);
	m3d::plane floor = m3d::plane(glm::vec3(0.0f, 1.0f, 0.0f), 0.0f);
	glm::vec3 pos = m3d::raycast(ray, floor);
	constexpr static float GRID_SIZE = 0.375f;
	pos.x = glm::floor(pos.x / GRID_SIZE) * GRID_SIZE;
	pos.z = glm::floor(pos.z / GRID_SIZE) * GRID_SIZE;

	for (int i = 0; i < 4; ++i) {
		entt::entity e = m_registry.create();
		m_registry.emplace<CPosition>(e, pos + glm::vec3(0.0f, 0.1f, 0.0f));
		m_registry.emplace<COrientation>(e, glm::vec3(0.0f, 1.0f, 0.0f), 0.0f);
		m_registry.emplace<CModel>(e, m_assetmanager.getMesh("res/models/raft/floor.obj"));
	}
}


