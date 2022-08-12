#include <GL/gl3w.h>
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

	m_camera = std::make_shared<Camera>(glm::vec3(6.0f, 5.5f, -6.0f));
	m_camera->setTarget(glm::vec3(0.0f, 0.0f, 0.0f));

	m_islandShader.load("res/glsl/proto/terrain_vert.glsl", sgl::shader::VERTEX);
	m_islandShader.load("res/glsl/proto/terrain_frag.glsl", sgl::shader::FRAGMENT);
	m_islandShader.compile();
	m_islandShader.link();

	Benchmark b;
	// TODO: use rawvertices directly, currently copying the same array twice. (rawvertices → vertices → new Mesh)
	Vertex* rawvertices = new Vertex[m_island.getMaxVertexCount()];
	size_t vertices_produced;
	m_island.polygonize(rawvertices, vertices_produced, glm::vec3(0.2f));
	std::vector<Vertex> vertices{rawvertices, rawvertices + vertices_produced};

	m_islandMesh = new Mesh(vertices);
	delete[] rawvertices;
	b.stop_and_print("marching cubes");
	fmt::print(" → used {} / {} vertices\n", vertices_produced, m_island.getMaxVertexCount());

	// Lights
	entt::entity light0 = m_registry.create(),
		         light1 = m_registry.create();
	m_registry.emplace<CPosition>(light0, glm::vec3(2.0f, 0.0f, 0.5f));
	m_registry.emplace<CPointLight>(light0, 0.5f, glm::vec3(1.0f, 0.0f, 0.0f));
	m_registry.emplace<CPosition>(light1, glm::vec3(2.0f, 0.0f, 1.0f));
	m_registry.emplace<CPointLight>(light1, 0.5f, glm::vec3(0.0f, 1.0f, 0.4f));

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
	Engine::Instance->dispatcher.sink<MouseButtonEvent>().disconnect<&MiniRaftScene::onMouseButtonInput>(this);
	delete m_islandMesh;

	std::for_each(m_rendersystems.begin(), m_rendersystems.end(), [](IRenderSystem* rsys) { delete rsys; });
	// TODO: double-freeing billboardRenderSystem
	//std::for_each(m_updatesystems.begin(), m_updatesystems.end(), [](IUpdateSystem* usys) { delete usys; });
}

void MiniRaftScene::onUpdate(float dt) {
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
	state.cull_face = true;
	Engine::Instance->graphics.setState(state);
	m_islandShader["uProj"] = m_camera->getProjection();
	m_islandShader["uView"] = m_camera->getView();
	m_islandShader["uModel"] = glm::translate(glm::mat4(1.0f), glm::vec3(-4.0f, 0.0f, -4.0f));
	m_islandShader["uTextureSide"] = 0;
	m_islandShader["uTextureTopdown"] = 0;
	m_islandShader["uTextureSideScale"] = 4.0f;
	m_islandShader["uTextureTopdownScale"] = 4.0f;
	static Texture* islandTexture = m_assetmanager.getTexture("res/images/textures/floor.png");
	glActiveTexture(GL_TEXTURE0);
	islandTexture->bind();
	islandTexture->setWrapMode(Texture::WrapMode::REPEAT);
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

	// write mouse position on floor into `pos`
	glm::vec2 mpos = Engine::Instance->window.getNormalizedMousePosition();
	m3d::ray ray = m_camera->raycast(mpos);
	m3d::plane floor = m3d::plane(glm::vec3(0.0f, 1.0f, 0.0f), 0.0f);
	glm::vec3 pos = m3d::raycast(ray, floor);

	if (event.button == GLFW_MOUSE_BUTTON_RIGHT) {
		constexpr static float GRID_SIZE = 0.375f;
		pos.x = glm::floor(pos.x / GRID_SIZE) * GRID_SIZE;
		pos.z = glm::floor(pos.z / GRID_SIZE) * GRID_SIZE;

		for (int i = 0; i < 4; ++i) {
			entt::entity e = m_registry.create();
			m_registry.emplace<CPosition>(e, pos + glm::vec3(0.0f, 0.1f, 0.0f));
			m_registry.emplace<COrientation>(e, glm::vec3(0.0f, 1.0f, 0.0f), 0.0f);
			m_registry.emplace<CModel>(e, m_assetmanager.getMesh("res/models/raft/floor.obj"), glm::vec3(0.75f));
		}
	} else {
		static Random rnd(0.0f, 1.0f);
		for (int i = 0; i < 4; ++i) {
			entt::entity e = m_registry.create();
			m_registry.emplace<CPosition>(e, pos + glm::vec3(0.0f, 0.2f, 0.0f));
			m_registry.emplace<CVelocity>(e, glm::vec3(rnd() * 2.0f - 1.0f, 1.75f, rnd() * 2.0f - 1.0f) * 0.01f);
			m_registry.emplace<CGravity>(e);

			const float rndModel = rnd();
			if (rndModel < 1.0f / 3.0f) {
				m_registry.emplace<COrientation>(e, glm::normalize(glm::vec3(rnd() - 0.5f, rnd() - 0.5f, rnd() - 0.5f)), rnd() * glm::two_pi<float>());
				m_registry.emplace<CModel>(e, m_assetmanager.getMesh("res/models/raft/barrel.obj"), glm::vec3(0.75f));
			} else if (rndModel < 2.0f / 3.0f) {
				m_registry.emplace<COrientation>(e, glm::normalize(glm::vec3(rnd() - 0.5f, rnd() - 0.5f, rnd() - 0.5f)), rnd() * glm::two_pi<float>());
				m_registry.emplace<CModel>(e, m_assetmanager.getMesh("res/models/raft/box.obj"), glm::vec3(0.75f));
			} else {
				static Texture* texture = m_assetmanager.getTexture("res/images/textures/dungeon.png");
				m_registry.emplace<CBillboard>(e, texture, glm::vec2(0.2f));
				m_registry.emplace<CTextureRegion>(e, glm::floor(rnd() * 5.0f) * 16.0f,
			                                   (9.0f + glm::floor(rnd() * 5.0f)) * 16.0f, 16.0f, 16.0f, 256, 256);
			}
		}
	}
}
