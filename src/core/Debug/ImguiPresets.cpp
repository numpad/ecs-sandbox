#include "Debug/ImguiPresets.hpp"

void imguiEntityEdit(entt::registry& registry, entt::entity entity) {
	if (entity == entt::null || !registry.valid(entity)) {
		ImGui::TextColored(ImVec4(1.0, 0.0, 0.0, 1.0), "No entity selected");
		return;
	}

// Add/remove components
#define COMP_TOGGLE(C)                                                                                                 \
	do {                                                                                                               \
		bool has_##C = registry.try_get<C>(entity);                                                                    \
		if (ImGui::Checkbox(#C, &has_##C)) {                                                                                  \
			if (!has_##C)                                                                                              \
				registry.remove<C>(entity);                                                                            \
			else                                                                                                       \
				registry.emplace<C>(entity);                                                                           \
		}                                                                                                              \
	} while (0)

	COMP_TOGGLE(CPosition);
	COMP_TOGGLE(CVelocity);
	// COMP_TOGGLE(CBillboard);
	COMP_TOGGLE(CGravity);
	COMP_TOGGLE(CSphereCollider);
	// COMP_TOGGLE(CKeyboardControllable);
	// COMP_TOGGLE(CRunningToTarget);
	COMP_TOGGLE(CHealth);
	COMP_TOGGLE(CDecal);
	COMP_TOGGLE(COrientation);
	COMP_TOGGLE(CTerrainCollider);

#undef COMP_TOGGLE

	// Edit component values
	if (registry.try_get<CPosition>(entity)) {
		if (ImGui::BeginMenu("CPosition")) {
			glm::vec3& pos = registry.get<CPosition>(entity).pos;
			ImGui::DragFloat3("Position", &pos[0], 0.005f);
			ImGui::EndMenu();
		}
	}
	if (registry.try_get<CVelocity>(entity)) {
		if (ImGui::BeginMenu("CVelocity")) {
			glm::vec3& vel = registry.get<CVelocity>(entity).vel;
			glm::vec3& acc = registry.get<CVelocity>(entity).acc;
			float& maxvel = registry.get<CVelocity>(entity).maxvel;

			ImGui::Text("velocity = %.5f", glm::length(vel));
			ImGui::DragFloat3("Velocity", &vel[0], 0.001f);
			ImGui::DragFloat3("Acceleration", &acc[0], 0.001f);
			ImGui::DragFloat("Max. velocity", &maxvel, 0.0001f);
			static glm::vec3 impulse;
			ImGui::DragFloat3("##apply_impulse", &impulse[0], 0.001f);
			ImGui::SameLine();
			if (ImGui::Button("Apply impulse")) {
				acc += impulse;
			}

			ImGui::EndMenu();
		}
	}
	if (registry.try_get<CBillboard>(entity)) {
		if (ImGui::BeginMenu("CBillboard")) {
			glm::vec2& size = registry.get<CBillboard>(entity).size;
			// glm::vec4 &texoffset = registry.get<CBillboard>(entity).texture_offset;
			ImGui::DragFloat2("Size", &size[0], 0.001f);
			ImGui::Text("TODO: handle CTextureRegion");
			// SliderFloat2("Texture offset", &texoffset[0], 0.0f, 1.0f);
			// SliderFloat2("Texture scale", &texoffset[2], 0.0f, 1.0f);
			static int srstart[2] = {0};
			const int texw = 256, texh = 256, tilesize = 16;

			bool a = ImGui::SliderInt2("Tile pos", srstart, 0, (texw / tilesize) - 1);
			// if (a) registry.get<CBillboard>(entity).setSubRect(
			//	float(srstart[0]) * float(tilesize), float(srstart[1]) * float(tilesize),
			//	float(tilesize), float(tilesize), texw, texh);

			ImGui::EndMenu();
		}
	}
	if (registry.try_get<CGravity>(entity)) {
		if (ImGui::BeginMenu("CGravity")) {
			ImGui::Text("Gravity: enabled");

			ImGui::EndMenu();
		}
	}
	if (registry.try_get<CSphereCollider>(entity)) {
		if (ImGui::BeginMenu("CSphereCollider")) {
			float& radius = registry.get<CSphereCollider>(entity).radius;
			float& force = registry.get<CSphereCollider>(entity).force;

			ImGui::DragFloat("Radius", &radius, 0.0005f, 0.0f);
			ImGui::DragFloat("Force", &force, 0.0005f);

			ImGui::EndMenu();
		}
	}
	if (registry.try_get<CKeyboardControllable>(entity)) {
		if (ImGui::BeginMenu("CKeyboardControllable")) {
			float& speed = registry.get<CKeyboardControllable>(entity).speed;

			ImGui::DragFloat("Speed", &speed, 0.000001f, 0.0000000001f, 0.1f, "%g");
			ImGui::EndMenu();
		}
	}
	if (registry.try_get<CRunningToTarget>(entity)) {
		if (ImGui::BeginMenu("CRunningToTarget")) {
			auto& rtt = registry.get<CRunningToTarget>(entity);
			ImGui::DragFloat("Speed", &rtt.force, 0.001f);
			ImGui::DragFloat("Goal range", &rtt.closeEnough, 0.001f);

			ImGui::EndMenu();
		}
	}

	if (registry.try_get<CHealth>(entity)) {
		if (ImGui::BeginMenu("CHealth")) {
			auto& health = registry.get<CHealth>(entity);
			ImGui::SliderInt("HP", &health.hp, 0, health.max_hp);
			ImGui::DragInt("Max. HP", &health.max_hp);
			ImGui::EndMenu();
		}
	}

	if (registry.try_get<CDecal>(entity)) {
		if (ImGui::BeginMenu("CDecal")) {
			auto& decal = registry.get<CDecal>(entity);
			ImGui::DragFloat3("Size##sizedecal", &decal.size[0], 0.001f);
			ImGui::EndMenu();
		}
	}

	if (registry.try_get<COrientation>(entity)) {
		if (ImGui::BeginMenu("COrientation")) {
			auto& orient = registry.get<COrientation>(entity);
			ImGui::SliderFloat3("Rotation", &orient.orientation[0], -1.f, 1.f);
			ImGui::SliderFloat("Amount", &orient.amount, 0.f, 1.f);
			ImGui::EndMenu();
		}
	}

	if (registry.try_get<CTerrainCollider>(entity)) {
		if (ImGui::BeginMenu("CTerrainCollider")) {
			auto& collider = registry.get<CTerrainCollider>(entity);
			ImGui::Text("On floor: ");
			ImGui::SameLine();
			if (collider.is_grounded) {
				ImGui::TextColored(ImVec4(0.0, 1.0, 0.0, 1.0), "yes");
			} else {
				ImGui::TextColored(ImVec4(1.0, 0.0, 0.0, 1.0), "no");
			}
			ImGui::Text("Dist to dir: ");
			ImGui::SameLine();
			ImGui::Text("%g", collider.dist_to_dir);
			ImGui::Text("Stair Height: ");
			ImGui::Text("%g", collider.stair_height);
			ImGui::EndMenu();
		}
	}

	if (ImGui::Button("Send Message")) {
		registry.ctx<entt::dispatcher>().trigger<WorldTextEvent>(entity, vec3(0.f, .25f, 0.f), L"Hello", 60 * 4);
	}
}

void imguiEntityEditor(entt::registry& registry, bool pickingactive, glm::vec3 crosspos) {
	if (ImGui::BeginMenu("Edit...")) {
		static bool pickingMode = false;
		static entt::entity selected = entt::null;
		if (selected == entt::null) {
			ImGui::Checkbox("Pick entity", &pickingMode);
		} else {
			if (ImGui::Button("Clear selection")) {
				selected = entt::null;
			}
		}
		ImGui::Separator();
		if (pickingMode && pickingactive) {
			// TODO: selected = world.getNearestEntity(crosspos);
			// iterate over registry<CPosition> → get closest
			pickingMode = false;
		}

		// entity editor
		imguiEntityEdit(registry, selected);
		ImGui::EndMenu();
	}
}

void imguiEntitySpawn(entt::registry& registry, bool spawn, glm::vec3 atpos) {

	// static glm::vec3 pos(0.0f);
	static glm::vec3 vel(0.0f);
	static glm::vec2 bbsize(0.2f, 0.2f);
	static bool bbrandom = true;
	static float rttforce = 0.01f;
	static float rttnear = 0.1f;
	static glm::vec3 rttpos(0.0f);
	static bool rttonlyonce = true;
	static float pressrad = 0.09f, pressforce = 0.01f;
	static float keycontrolspeed = 0.0015f;
	static glm::vec3 spawnpoint(0.0f);
	static char texpath[512] = "res/images/textures/dungeon.png";
	static glm::ivec2 tiles(16, 16), tilepos(1, 10);
	static int spawnamount = 1;
	static float spawnveloff = 0.02f;
	static int max_hp = 10;
	static glm::vec3 size(1.f);
	static char decal_texpath[512] = "res/images/decals/explosion.png";
	static glm::vec4 subrect(0.f, 0.f, 1.f, 1.f);
	static glm::vec3 orient(0.f, 1.f, 0.f);
	static float orient_amount = 0.f;

	static bool haspos = true, hasvel = true, hasgrav = true, hasbb = true, hasruntt = false, runttToPlayer = false,
	            haspresser = false, haskeyboard = false, hasspawn = false, hasjumper = true, hashealth = true,
	            hasdecal = false, hasorientation = false, hasterraincollider = true;

	if (ImGui::BeginMenu("Spawn...")) {
		ImGui::Checkbox("CPosition", &haspos);
		ImGui::Checkbox("CVelocity", &hasvel);
		ImGui::Checkbox("CGravity", &hasgrav);
		ImGui::Checkbox("CBillboard", &hasbb);
		ImGui::Checkbox("CRunToTarget", &hasruntt);
		ImGui::Checkbox("CSphereCollider", &haspresser);
		ImGui::Checkbox("CKeyboardController", &haskeyboard);
		ImGui::Checkbox("CSpawnPoint", &hasspawn);
		ImGui::Checkbox("CJumpTimer", &hasjumper);
		ImGui::Checkbox("CHealth", &hashealth);
		ImGui::Checkbox("CDecal", &hasdecal);
		ImGui::Checkbox("COrientation", &hasorientation);
		ImGui::Checkbox("CTerrainCollider", &hasterraincollider);

		ImGui::Separator();

		if (hasvel)
			if (ImGui::BeginMenu("CVelocity")) {
				ImGui::DragFloat3("##CVelocity", &vel[0], 0.001f);
				ImGui::EndMenu();
			}
		// if (hasgrav) Text("Gravity: enabled");
		if (hasbb) {
			if (ImGui::BeginMenu("CBillboard")) {
				ImGui::DragFloat2("Size", &bbsize[0], 0.001f);
				ImGui::InputText("Texture", texpath, 512);
				ImGui::DragInt2("Tiles", &tiles[0], 1);
				if (!bbrandom)
					ImGui::DragInt2("Tile pos", &tilepos[0], 1);
				ImGui::Checkbox("Random?", &bbrandom);

				ImGui::EndMenu();
			}
		}
		if (hasruntt) {
			if (ImGui::BeginMenu("CRunningToTarget")) {
				ImGui::Checkbox("Only once", &rttonlyonce);
				ImGui::Checkbox("Player", &runttToPlayer);
				if (!runttToPlayer) {
					ImGui::DragFloat3("Position", &rttpos[0], 0.001f);
				}
				ImGui::SliderFloat("Speed", &rttforce, 0.0f, 0.01f);
				ImGui::SliderFloat("Near", &rttnear, 0.0f, 1.0f);

				ImGui::EndMenu();
			}
		}
		if (haspresser) {
			if (ImGui::BeginMenu("CSphereCollider")) {
				ImGui::DragFloat("Radius", &pressrad, 0.0f, 0.1f);
				ImGui::DragFloat("Force", &pressforce, 0.0f, 0.05f);
				ImGui::EndMenu();
			}
		}
		if (haskeyboard) {
			if (ImGui::BeginMenu("CKeyboardControllable")) {
				ImGui::DragFloat("control speed", &keycontrolspeed, 0.0005f, 0.0035f);
				ImGui::EndMenu();
			}
		}
		if (hasspawn) {
			if (ImGui::BeginMenu("CSpawnPoint")) {
				ImGui::DragFloat3("SpawnPoint", &spawnpoint[0], 0.001f);
				ImGui::EndMenu();
			}
		}
		if (hasjumper) {
		}
		if (hashealth) {
			if (ImGui::BeginMenu("CHealth")) {
				ImGui::DragInt("Max. HP", &max_hp, 1);
				ImGui::EndMenu();
			}
		}
		if (hasdecal) {
			if (ImGui::BeginMenu("CDecal")) {
				ImGui::DragFloat3("Size", &size[0], 0.001f);
				ImGui::InputText("Path", decal_texpath, 512);
				ImGui::SliderFloat4("Subrect", &subrect[0], 0.f, 1.f);
				ImGui::EndMenu();
			}
		}
		if (hasorientation) {
			if (ImGui::BeginMenu("COrientation")) {
				ImGui::SliderFloat3("Orient", &orient[0], -1.f, 1.f);
				ImGui::SliderFloat("Amount", &orient_amount, 0.f, 1.f);
				ImGui::EndMenu();
			}
		}
		if (hasterraincollider) {
		}

		ImGui::Separator();
		ImGui::SliderInt("Amount", &spawnamount, 1, 50);
		ImGui::SliderFloat("Velocity", &spawnveloff, 0.0f, 0.05f);

		ImGui::EndMenu();
	}

	//
	// do spawning
	static Random r;
	if (spawn) {
		for (int i = 0; i < spawnamount; ++i) {
			auto entity = registry.create();
			if (haspos)
				registry.emplace<CPosition>(entity, atpos);
			if (hasvel)
				registry.emplace<CVelocity>(entity, m3d::randomizeVec3(vel, spawnveloff));
			if (hasgrav)
				registry.emplace<CGravity>(entity);
			if (hasbb) {
				if (bbrandom) {
					tilepos.x = int(r() * 5.f);
					tilepos.y = 11 + int(r() * 3.f);
				}
				// Texture *texture = world.getAssetManager().getTiledTexture(texpath, tiles.x, tiles.y, tilepos.x,
				// tilepos.y); registry.emplace<CBillboard>(entity, texture, bbsize);
				// registry.emplace<CTextureRegion>(entity, texture->getSubRect());
				fmt::print(fmt::fg(fmt::terminal_color::red), "'world.getAssetManager()' : NOT IMPLEMENTED\n");
				// TODO: get assetmanager
			}
			if (hasruntt) {
				if (runttToPlayer) {
					// TODO: registry.emplace<CRunningToTarget>(entity, world.getPlayer(), rttforce, rttnear,
					// rttonlyonce);
					fmt::print(fmt::fg(fmt::terminal_color::red), "'world.getPlayer()' : NOT IMPLEMENTED\n");
				} else {
					registry.emplace<CRunningToTarget>(entity, rttpos, rttforce, rttnear, rttonlyonce);
				}
			}
			if (haspresser)
				registry.emplace<CSphereCollider>(entity, pressrad, pressforce);
			if (haskeyboard)
				registry.emplace<CKeyboardControllable>(entity, keycontrolspeed);
			if (hasspawn)
				registry.emplace<CSpawnPoint>(entity, spawnpoint);
			if (hasjumper)
				registry.emplace<CJumpTimer>(entity);
			if (hashealth)
				registry.emplace<CHealth>(entity, max_hp);
			// TODO: getAssetManager
			// if (hasdecal) registry.emplace<CDecal>(entity, size, world.getAssetManager().getTexture(decal_texpath),
			// subrect);
			if (hasdecal)
				fmt::print(fmt::fg(fmt::terminal_color::red), "'world.getAssetManager()' : NOT IMPLEMENTED\n");
			if (hasorientation)
				registry.emplace<COrientation>(entity, orient, orient_amount);
			if (hasterraincollider)
				registry.emplace<CTerrainCollider>(entity, false);
		}
	}
}

void imguiRenderMenuBar(Engine* engine, entt::registry& registry, glm::vec3& crosspos, float msPerFrame) {
	GLFWwindow* window = engine->window;
	if (ImGui::BeginMainMenuBar()) {
		// Spawn entity
		imguiEntitySpawn(registry, (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS), crosspos);
		// Edit entity
		imguiEntityEditor(registry, (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS), crosspos);

		// Shaders
		static bool shaders_reload = false;
		if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
			shaders_reload = true;
		if (ImGui::BeginMenu("Shader")) {
			if (ImGui::MenuItem("Reload all", "P")) {
				shaders_reload = true;
			}

			ImGui::EndMenu();
		}
		if (shaders_reload) {
			shaders_reload = false;

			sgl::shader* chunkShader = Blackboard::read<sgl::shader>("chunkShader");
			sgl::shader* billboardShader = Blackboard::read<sgl::shader>("billboardShader");
			sgl::shader* deferredShader = Blackboard::read<sgl::shader>("deferredShader");
			sgl::shader* decalShader = Blackboard::read<sgl::shader>("decalShader");
			if (chunkShader)
				chunkShader->reload();
			if (billboardShader)
				billboardShader->reload();
			if (deferredShader)
				deferredShader->reload();
			if (decalShader)
				decalShader->reload();
		}

		// Settings
		if (ImGui::BeginMenu("Settings")) {
			static bool settings_vsync = true;
			if (ImGui::Checkbox("V-Sync", &settings_vsync))
				glfwSwapInterval(settings_vsync ? 1 : 0);
			static bool settings_wireframe = false;
			if (ImGui::Checkbox("Wireframe", &settings_wireframe)) {
				glPolygonMode(GL_FRONT_AND_BACK, settings_wireframe ? GL_LINE : GL_FILL);
			}
			ImGui::EndMenu();
		}

		ImGui::Separator();

		ImGui::Text("%g ms / frame", msPerFrame);
		ImGui::EndMainMenuBar();
	}
}

void imguiLuaJitConsole(lua_State* L) {
	if (ImGui::Begin("LuaJIT")) {
		constexpr size_t buf_size = 2048;
		static char buf[buf_size] = "local ffi = require('ffi')\nlocal vec3 = require('Vec3')\nffi.cdef[[\n  typedef "
		                            "struct { float x, y, z; } vec3;\n  \n]]\n\n";
		ImVec2 win_size = ImGui::GetWindowSize();
		win_size.y -= 58;
		ImGui::InputTextMultiline("##editor", buf, buf_size, win_size);
		if (ImGui::Button("Evaluate")) {
			if (luaL_dostring(L, buf) != 0) {
				std::cerr << "[LUA] Error: " << lua_tostring(L, -1) << std::endl;
				lua_pop(L, 1);
			}
		}
	}
	ImGui::End();
}

void imguiGamepadInfo() {
	if (ImGui::Begin("Gamepad Info")) {
		ImGui::BeginTable("gamepad_info", 3, ImGuiTableFlags_Hideable | ImGuiTableFlags_Borders);

		for (int i = GLFW_JOYSTICK_1; i <= GLFW_JOYSTICK_LAST; ++i) {
			ImGui::TableNextRow();

			ImGui::TableNextColumn();
			glfwJoystickIsGamepad(i) ? ImGui::TextColored(ImVec4(0.35, 0.25, 0.74, 1.0), "Gamepad #%d", i)
			                         : ImGui::Text("Joystick #%d", i);

			ImGui::TableNextColumn();
			if (glfwJoystickPresent(i)) {
				ImGui::TextColored(ImVec4(0.54, 0.7, 0.55, 1.0), "(connected)");
			} else {
				ImGui::TextColored(ImVec4(0.64, 0.4, 0.4, 1.0), "(disconnected)");
			}

			ImGui::TableNextColumn();
			ImGui::Text("\"%s\"", glfwGetGamepadName(i));
		}

		ImGui::EndTable();
	}
	ImGui::End();
}
