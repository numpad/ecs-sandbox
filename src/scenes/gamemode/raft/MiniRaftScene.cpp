#include "scenes/gamemode/raft/MiniRaftScene.hpp"
#include "Assets/Loaders/ObjLoader.hpp"
#include "Graphics/GLState.hpp"
#include "RenderObject/GBuffer.hpp"
#include "Util/File.hpp"
#include "Util/Math3d.hpp"
#include "Util/Random.hpp"
#include "Util/Benchmark.hpp"
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

static inline glm::vec3 limitMag(glm::vec3 of, glm::vec3 to) {
	return glm::normalize(of) * glm::min(glm::length(to), glm::length(of));
}

bool MiniRaftScene::onCreate() {
	m_registry.set<entt::dispatcher>();
	Engine::Instance->dispatcher.sink<MouseButtonEvent>().connect<&MiniRaftScene::onMouseButtonInput>(this);

	m_camera = std::make_shared<Camera>(glm::vec3(3.0f, 4.5f, -5.0f));
	m_camera->setTarget(glm::vec3(0.0f, 0.1f, 0.0f));

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
	
	m_islandShader.load("res/glsl/proto/terrain_vert.glsl", sgl::shader::VERTEX);
	m_islandShader.load("res/glsl/proto/terrain_frag.glsl", sgl::shader::FRAGMENT);
	m_islandShader.compile();
	m_islandShader.link();

	Benchmark b;
	Vertex* rawvertices = m_island.polygonize();
	std::vector<Vertex> vertices{rawvertices, rawvertices + (m_island.size.x * m_island.size.y * m_island.size.z * 12)};
	m_islandMesh = new Mesh(vertices);
	b.stop_and_print("marching cubes");

	Random rnd(0.0f, 1.0f);
	for (float i = 0.0f; i < glm::two_pi<float>(); i += glm::two_pi<float>() / 20.0f) {
		float o = rnd() * 0.2f;
		float x = cos(i + o);
		float z = sin(i + o);
		const float l = 2.8f + rnd() * 0.2f;
		auto e = m_registry.create();
		m_registry.emplace<CPosition>(e, glm::vec3(x * l, -0.04f, z * l));
		
		float model = rnd();
		if (model < 0.5f) {
			m_registry.emplace<CModel>(e, m_assetmanager.getMesh("res/models/raft/rockA.obj"), glm::vec3(rnd() * 0.5f + 0.5f));
		} else {
			m_registry.emplace<CModel>(e, m_assetmanager.getMesh("res/models/raft/rockB.obj"), glm::vec3(rnd() * 0.5f + 0.5f));
		}

		m_registry.emplace<COrientation>(e, glm::vec3(0.0f, 1.0f, 0.0f), rnd() * glm::two_pi<float>());
	}

	return true;
}

void MiniRaftScene::onDestroy() {
	Engine::Instance->dispatcher.sink<MouseButtonEvent>().disconnect<&MiniRaftScene::onMouseButtonInput>(this);

	std::for_each(m_updatesystems.begin(), m_updatesystems.end(), [](IUpdateSystem* usys) {
		delete usys;
	});
	// TODO: double-freeing billboardRenderSystem
	// std::for_each(m_rendersystems.begin(), m_rendersystems.end(), [](IRenderSystem* rsys) { delete rsys; });
}

void MiniRaftScene::onUpdate(float dt) {
#ifndef NDEBUG
	m3d::ray ray = m_camera->raycast(Engine::Instance->window.getNormalizedMousePosition());
	m3d::plane floor = m3d::plane(glm::vec3(0.0f, 1.0f, 0.0f), 0.0f);
	glm::vec3 pos = m3d::raycast(ray, floor);
	
	static Random rnd(-1.0f, 1.0f);
	float _x = rnd();
	float _y = -1;
	float _r = (rnd() * 0.5f + 0.5f) * 0.04f + 0.012f;
	float _px = pos.x;
	float _py = 0.0f;
	float _pz = pos.z;

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
				m_registry.emplace<CModel>(e, m_assetmanager.getMesh("res/models/raft/barrel.obj"), glm::vec3(0.5f));
			else
				m_registry.emplace<CModel>(e, m_assetmanager.getMesh("res/models/raft/box.obj"), glm::vec3(0.6f));
		}
	}

#endif

	std::for_each(m_updatesystems.begin(), m_updatesystems.end(), [dt](auto& usys) {
		usys->update(dt);
	});

	m_registry.view<CPosition, CVelocity, const CGravity>().each(
	    [](entt::entity entity, CPosition& cpos, CVelocity& cvel, const auto& cgravity) {
			// make entites float
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
	
	GLState state;
	state.depth_test = true;
	state.depth_write = true;
	Engine::Instance->graphics.setState(state);
	m_islandShader["uProj"] = m_camera->getProjection();
	m_islandShader["uView"] = m_camera->getView();
	m_islandShader["uModel"] = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.3f, 0.0f));
	m_islandShader["uTextureSide"] = 0;
	m_islandShader["uTextureTopdown"] = 0;
	m_islandShader["uTextureSideScale"] = 1.0f;
	m_islandShader["uTextureTopdownScale"] = 1.0f;
	static Texture* islandTexture = m_assetmanager.getTexture("res/images/textures/floor.png");
	glActiveTexture(GL_TEXTURE0);
	islandTexture->bind();
	m_islandMesh->draw(m_islandShader);

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
	if (!event.is_down)
		return;

	glm::vec2 mpos = Engine::Instance->window.getNormalizedMousePosition();
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
		m_registry.emplace<CModel>(e, m_assetmanager.getMesh("res/models/raft/floor.obj"), glm::vec3(0.75f));
	}
}
