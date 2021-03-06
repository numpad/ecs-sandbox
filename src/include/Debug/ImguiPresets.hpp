#pragma once

#include <imgui/imgui.h>
#include <entt/entt.hpp>

#include <Util/Random.hpp>
#include <Util/Blackboard.hpp>

void imguiEntityEdit(entt::registry &registry, entt::entity entity) {
	using namespace ImGui;
	
	if (entity == entt::null || !registry.valid(entity)) {
		TextColored(ImVec4(1.0, 0.0, 0.0, 1.0), "null-Entity");
		return;
	}
	
	if (registry.has<CPosition>(entity)) {
		glm::vec3 &pos = registry.get<CPosition>(entity).pos;
		DragFloat3("pos", &pos[0], 0.005f);
		SameLine();
		if (Button("X##1")) {
			registry.remove<CPosition>(entity);
		}
	}
	if (registry.has<CVelocity>(entity)) {
		glm::vec3 &vel = registry.get<CVelocity>(entity).vel;
		glm::vec3 &acc = registry.get<CVelocity>(entity).acc;
		float &maxvel = registry.get<CVelocity>(entity).maxvel;
		
		Text("velocity = %.5f", glm::length(vel));
		DragFloat3("vel", &vel[0], 0.001f);
		DragFloat3("acceleration", &acc[0], 0.001f);
		DragFloat("max velocity", &maxvel, 0.0001f);
		static glm::vec3 impulse;
		DragFloat3("##apply_impulse", &impulse[0], 0.001f);
		SameLine();
		if (Button("impulse")) {
			acc += impulse;
		}
		SameLine();
		if (Button("X##2")) {
			registry.remove<CVelocity>(entity);
		}
	}
	if (registry.has<CBillboard>(entity)) {
		glm::vec2 &size = registry.get<CBillboard>(entity).size;
		glm::vec3 &color = registry.get<CBillboard>(entity).color;
		glm::vec4 &texoffset = registry.get<CBillboard>(entity).texture_offset;
		DragFloat2("size", &size[0], 0.001f);
		ColorEdit3("tint", &color[0]);
		SliderFloat2("texoffset", &texoffset[0], 0.0f, 1.0f);
		SliderFloat2("texscale", &texoffset[2], 0.0f, 1.0f);
		static int srstart[2] = {0};
		const int texw = 256, texh = 256, tilesize = 16;
		
		bool a = SliderInt2("tilepos", srstart, 0, (texw / tilesize) - 1);
		if (a) registry.get<CBillboard>(entity).setSubRect(
			float(srstart[0]) * float(tilesize), float(srstart[1]) * float(tilesize),
			float(tilesize), float(tilesize), texw, texh);
		
		SameLine();
		if (Button("X##3")) {
			registry.remove<CBillboard>(entity);
		}
	}
	if (registry.has<CGravity>(entity)) {
		Text("Gravity: enabled");
		SameLine();
		if (Button("X##4")) {
			registry.remove<CGravity>(entity);
		}
	}
	if (registry.has<CSphereCollider>(entity)) {
		float &radius = registry.get<CSphereCollider>(entity).radius;
		float &force = registry.get<CSphereCollider>(entity).force;
		
		DragFloat("radius", &radius, 0.0005f, 0.0f);
		DragFloat("force", &force, 0.0005f);
		SameLine();
		if (Button("X##5")) {
			registry.remove<CSphereCollider>(entity);
		}
	}
	if (registry.has<CKeyboardControllable>(entity)) {
		float &speed = registry.get<CKeyboardControllable>(entity).speed;
		
		DragFloat("char speed", &speed, 0.000001f, 0.0000000001f, 0.1f, "%g");
		SameLine();
		if (Button("X##6")) {
			registry.remove<CKeyboardControllable>(entity);
		}
	}
	if (registry.has<CRunningToTarget>(entity)) {
		auto &rtt = registry.get<CRunningToTarget>(entity);
		DragFloat("speed", &rtt.force, 0.001f);
		DragFloat("closeEnough", &rtt.closeEnough, 0.001f);
		if (Button("X##7")) {
			registry.remove<CRunningToTarget>(entity);
		}
	}
	
	if (registry.has<CHealth>(entity)) {
		auto &health = registry.get<CHealth>(entity);
		SliderInt("health", &health.hp, 0, health.max_hp);
		if (Button("X##8")) {
			registry.remove<CHealth>(entity);
		}
	}

	if (registry.has<CDecal>(entity)) {
		Text("Decal");
		auto &decal = registry.get<CDecal>(entity);
		DragFloat3("size##sizedecal", &decal.size[0], 0.001f);
		if (Button("X##9")) {
			registry.remove<CDecal>(entity);
		}
	}

	if (registry.has<COrientation>(entity)) {
		auto &orient = registry.get<COrientation>(entity);
		SliderFloat3("Orient", &orient.orientation[0], -1.f, 1.f);
		SliderFloat("Amount", &orient.amount, 0.f, 1.f);
		if (Button("X##10")) {
			registry.remove<COrientation>(entity);
		}
	}

	if (Button("Send Message")) {
		registry.ctx<entt::dispatcher>().trigger<WorldTextEvent>(entity, vec3(0.f, .25f, 0.f), L"Hello", 60 * 4);
	}
}

void imguiEntitySpawn(World &world, bool spawn, glm::vec3 atpos) {
	using namespace ImGui;
	entt::registry &registry = world.getRegistry();
	
	//static glm::vec3 pos(0.0f);
	static glm::vec3 vel(0.0f);
	static glm::vec2 bbsize(0.2f, 0.2f);
	static glm::vec3 bbcolor(1.0f);
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

	static bool haspos = true,
				hasvel = true,
				hasgrav = true,
				hasbb = true,
				hasruntt = false,
				runttToPlayer = false,
				haspresser = false,
				haskeyboard = false,
				hasspawn = false,
				hasjumper = true,
				hashealth = true,
				hasdecal = false,
				hasorientation = false;
	
	if (BeginMenu("Spawn...")) {
		Checkbox("CPosition", &haspos);
		Checkbox("CVelocity", &hasvel);
		Checkbox("CGravity", &hasgrav);
		Checkbox("CBillboard", &hasbb);
		Checkbox("CRunToTarget", &hasruntt);
		Checkbox("CSphereCollider", &haspresser);
		Checkbox("CKeyboardController", &haskeyboard);
		Checkbox("CSpawnPoint", &hasspawn);
		Checkbox("CJumpTimer", &hasjumper);
		Checkbox("CHealth", &hashealth);
		Checkbox("CDecal", &hasdecal);
		Checkbox("COrientation", &hasorientation);
		
		Separator();
		
		if (hasvel) if (BeginMenu("CVelocity")) { DragFloat3("##CVelocity", &vel[0], 0.001f); ImGui::EndMenu(); }
		//if (hasgrav) Text("Gravity: enabled");
		if (hasbb) {
			if (BeginMenu("CBillboard")) {
				DragFloat2("Size", &bbsize[0], 0.001f);
				ColorEdit3("Color", &bbcolor[0]);
				InputText("Texture", texpath, 512);
				DragInt2("Tiles", &tiles[0], 1);
				if (!bbrandom)
					DragInt2("Tile pos", &tilepos[0], 1);
				Checkbox("Random?", &bbrandom);
				
				ImGui::EndMenu();
			}
		}
		if (hasruntt) {
			if (BeginMenu("CRunningToTarget")) {
				Checkbox("Only once", &rttonlyonce);
				Checkbox("Player", &runttToPlayer);
				if (!runttToPlayer) {
					DragFloat3("Position", &rttpos[0], 0.001f);
				}
				SliderFloat("Speed", &rttforce, 0.0f, 0.01f);
				SliderFloat("Near", &rttnear, 0.0f, 1.0f);
				
				ImGui::EndMenu();
			}
		}
		if (haspresser) {
			if (BeginMenu("CSphereCollider")) {
				DragFloat("Radius", &pressrad, 0.0f, 0.1f);
				DragFloat("Force", &pressforce, 0.0f, 0.05f);
				ImGui::EndMenu();
			}
		}
		if (haskeyboard) {
			if (BeginMenu("CKeyboardControllable")) {
				DragFloat("control speed", &keycontrolspeed, 0.0005f, 0.0035f);
				ImGui::EndMenu();
			}
		}
		if (hasspawn) {
			if (BeginMenu("CSpawnPoint")) {
				DragFloat3("SpawnPoint", &spawnpoint[0], 0.001f);
				ImGui::EndMenu();
			}
		}
		if (hasjumper) {
			
		}
		if (hashealth) {
			if (BeginMenu("CHealth")) {
				DragInt("Max. HP", &max_hp, 1);
				ImGui::EndMenu();
			}
		}
		if (hasdecal) {
			if (BeginMenu("CDecal")) {
				DragFloat3("Size", &size[0], 0.001f);
				InputText("Path", decal_texpath, 512);
				SliderFloat4("Subrect", &subrect[0], 0.f, 1.f);
				ImGui::EndMenu();
			}
		}
		if (hasorientation) {
			if (BeginMenu("COrientation")) {
				SliderFloat3("Orient", &orient[0], -1.f, 1.f);
				SliderFloat("Amount", &orient_amount, 0.f, 1.f);
				ImGui::EndMenu();
			}
		}
		
		Separator();
		SliderInt("Amount", &spawnamount, 1, 50);
		SliderFloat("Velocity", &spawnveloff, 0.0f, 0.05f);
		
		ImGui::EndMenu();
	}
	
	
	// 
	// do spawning
	static Random r;
	if (spawn) {
		for (int i = 0; i < spawnamount; ++i) {
			auto entity = registry.create();
			if (haspos) registry.emplace<CPosition>(entity, atpos);
			if (hasvel) registry.emplace<CVelocity>(entity,
				m3d::randomizeVec3(vel, spawnveloff));
			if (hasgrav) registry.emplace<CGravity>(entity);
			if (hasbb) {
				if (bbrandom) {
					tilepos.x = int(r()*5.f);
					tilepos.y = 11+int(r()*3.f);
				}
				registry.emplace<CBillboard>(entity,
					world.getAssetManager().getTiledTexture(texpath, tiles.x, tiles.y, tilepos.x, tilepos.y),
					bbsize, bbcolor);
			}
			if (hasruntt) {
				if (runttToPlayer) {
					registry.emplace<CRunningToTarget>(entity, world.getPlayer(), rttforce, rttnear, rttonlyonce);
				} else {
					registry.emplace<CRunningToTarget>(entity, rttpos, rttforce, rttnear, rttonlyonce);
				}
			}
			if (haspresser) registry.emplace<CSphereCollider>(entity, pressrad, pressforce);
			if (haskeyboard) registry.emplace<CKeyboardControllable>(entity, keycontrolspeed);
			if (hasspawn) registry.emplace<CSpawnPoint>(entity, spawnpoint);
			if (hasjumper) registry.emplace<CJumpTimer>(entity);
			if (hashealth) registry.emplace<CHealth>(entity, max_hp);
			if (hasdecal) registry.emplace<CDecal>(entity, size, world.getAssetManager().getTexture(decal_texpath), subrect);
			if (hasorientation) registry.emplace<COrientation>(entity, orient, orient_amount);
		}
	}
}

void imguiRenderMenuBar(GLFWwindow *window, World &world, glm::vec3 &crosspos, std::shared_ptr<Camera> topdown, std::shared_ptr<Camera> camera, float &msPerFrame, int &settings_attachment) {
	if (ImGui::Begin("world")) {
		static bool pickingMode = false;
		ImGui::Checkbox("Entity picker", &pickingMode);
		static entt::entity selected = entt::null;
		if (pickingMode && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
			selected = world.getNearestEntity(crosspos);
			pickingMode = false;
		}
		
		// entity editor
		imguiEntityEdit(world.getRegistry(), selected);
	}
	ImGui::End();

	if (ImGui::BeginMainMenuBar()) {
		// Spawn entity
		imguiEntitySpawn(world, (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS), crosspos);
		// Shaders
		static bool shaders_reload = false;
		if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) shaders_reload = true;
		if (ImGui::BeginMenu("Shader")) {
			if (ImGui::MenuItem("Reload all", "P")) {
				shaders_reload = true;
			}
			
			ImGui::EndMenu();
		}
		if (shaders_reload) {
			shaders_reload = false;
			
			sgl::shader *chunkShader = Blackboard::read<sgl::shader>("chunkShader");
			sgl::shader *billboardShader = Blackboard::read<sgl::shader>("billboardShader");
			sgl::shader *deferredShader = Blackboard::read<sgl::shader>("deferredShader");
			sgl::shader *decalShader = Blackboard::read<sgl::shader>("decalShader");
			if (chunkShader) chunkShader->reload();
			if (billboardShader) billboardShader->reload();
			if (deferredShader) deferredShader->reload();
			if (decalShader) decalShader->reload();
		}
		
		// Cameras
		if (ImGui::BeginMenu("Rendering")) {
			// which camera
			ImGui::Text("Camera");
			static int settings_camera = 0;
			bool settings_changed = false;
			if (ImGui::RadioButton("Default", settings_camera == 0)) { settings_camera = 0; settings_changed = true; }
			if (ImGui::RadioButton("Camera #1 (topdown)", settings_camera == 1)) { settings_camera = 1; settings_changed = true; }
			if (settings_changed) {
				settings_changed = false;
				switch (settings_camera) {
					case 1: world.setCamera(topdown); break;
					default: world.setCamera(camera); break;
				};
			}
			ImGui::Separator();
			// which framebuffer attachment
			ImGui::Text("Framebuffer");
			static bool settings_attachment_change = false;
			// defined above because of scope:
			//static int settings_attachment = GL_COLOR_ATTACHMENT0;
			if (ImGui::RadioButton("- Result -", &settings_attachment, 0)) { settings_attachment_change = true; };
			if (ImGui::RadioButton("Color Buffer", &settings_attachment, GL_COLOR_ATTACHMENT0)) { settings_attachment_change = true; };
			if (ImGui::RadioButton("Position Buffer", &settings_attachment, GL_COLOR_ATTACHMENT1)) { settings_attachment_change = true; };
			if (ImGui::RadioButton("Normal Buffer", &settings_attachment, GL_COLOR_ATTACHMENT2)) { settings_attachment_change = true; };
			if (ImGui::RadioButton("Depth Buffer", &settings_attachment, GL_COLOR_ATTACHMENT3)) { settings_attachment_change = true; };
			
			// switch to darkmode when viewing result rendering.
			if (settings_attachment_change) {
				settings_attachment_change = false;
				if (settings_attachment == 0) {
					ImGui::StyleColorsDark();
				} else {
					ImGui::StyleColorsLight();
				}
			}
			
			ImGui::EndMenu();
		}
		
		// Settings
		if (ImGui::BeginMenu("Settings")) {
			static bool settings_vsync = true;
			if (ImGui::Checkbox("V-Sync", &settings_vsync)) glfwSwapInterval(settings_vsync ? 1 : 0);
			static bool settings_wireframe = false;
			if (ImGui::Checkbox("Wireframe", &settings_wireframe)) {
				glPolygonMode(GL_FRONT_AND_BACK, settings_wireframe ? GL_LINE : GL_FILL);
			}
			if (ImGui::MenuItem("Reset")) {
				world.resetEntities();
			}
			ImGui::EndMenu();
		}
		
		ImGui::Separator();
		
		ImGui::Text("%g ms / frame", msPerFrame);
		if (ImGui::Button("load world")) {
			world.load();
		}
		ImGui::EndMainMenuBar();
	}
}
