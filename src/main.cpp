#include "config.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <FastNoise/FastNoise.h>
#include <stb/stb_image.h>
#include <util/sgl_shader.hpp>
#include <imgui/imgui.h>
#include <imgui/examples/imgui_impl_glfw.h>
#include <imgui/examples/imgui_impl_opengl3.h>
#include <entt/entt.hpp>
#include <Util/Math3d.hpp>

#include <Assets/AssetManager.hpp>
#include <World.hpp>

void onResize(GLFWwindow *, int width, int height) {
	glViewport(0, 0, width, height);
}

void APIENTRY glDebugOutput(GLenum source,
                            GLenum type,
                            GLuint id,
                            GLenum severity,
                            GLsizei length,
                            const GLchar *message,
                            const void *userParam)
{
	(void)length;
	(void)userParam;
	
    // ignore non-significant error/warning codes
    if(id == 131169 || id == 131185 || id == 131218 || id == 131204) return; 

    std::cout << "---------------" << std::endl;
    std::cout << "Debug message (" << id << "): " <<  message << std::endl;

    switch (source)
    {
        case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
        case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
        case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
    } std::cout << std::endl;

    switch (type)
    {
        case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break; 
        case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
        case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
        case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
        case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
        case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
        case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
    } std::cout << std::endl;
    
    switch (severity)
    {
        case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
        case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
        case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
        case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
    } std::cout << std::endl;
    std::cout << std::endl;
}

inline void imguiInit(GLFWwindow *window) {
	#if CFG_IMGUI_ENABLED
		// init imgui
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO &io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		ImGui::StyleColorsLight();
		// imgui glfw init
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 450");
	#endif
}
inline void imguiBeforeFrame() {
	#if CFG_IMGUI_ENABLED
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	#endif
}
inline void imguiRender() {
	#if CFG_IMGUI_ENABLED
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	#endif
}
inline void imguiDestroy() {
	#if CFG_IMGUI_ENABLED
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	#endif
}

bool initGL() {
	#if CFG_DEBUG
		const char *cfg_debug_state = "DEBUG";
	#else
		const char *cfg_debug_state = "Release build";
	#endif
	printf("%s version: %d.%d (%s)\n", CFG_PROJECT_NAME, CFG_VERSION_MAJOR, CFG_VERSION_MINOR, cfg_debug_state);
	
	if (!glfwInit()) {
		fprintf(stderr, "glfwInit() failed.\n");
		return false;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_SAMPLES, 4);
	
	#if CFG_DEBUG
		// TODO: fix for my window manager
		glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	#endif
	
	/* request debug context */
	#if CFG_DEBUG
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, (CFG_DEBUG ? GLFW_TRUE : GLFW_FALSE));
	#endif
	
	return true;
}

bool initWindow(GLFWwindow **window, int width, int height) {
	*window = glfwCreateWindow(width, height, "main", nullptr, nullptr);
	glfwMakeContextCurrent(*window);
	glfwSwapInterval(1);
	glfwSetFramebufferSizeCallback(*window, onResize);

	if (gl3wInit()) {
		fprintf(stderr, "gl3wInit() failed.\n");
		return false;
	}
	printf("OpenGL %s, GLSL %s\n",
		glGetString(GL_VERSION),
		glGetString(GL_SHADING_LANGUAGE_VERSION));
	
	// msaa
	//glEnable(GL_MULTISAMPLE);
	//glEnable(GL_SAMPLE_SHADING);
	//glMinSampleShading(1.0f);
	
	// depth test
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	
	// blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);
	
	/* check if debug enabled */
	#if CFG_DEBUG
	GLint flags;
	glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
	if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
		printf("OpenGL debug context enabled!\n");
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(glDebugOutput, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
	}
	#endif
	
	imguiInit(*window);
	
	return true;
}

float getWindowAspectRatio(GLFWwindow *window) {
	int iw, ih;
	glfwGetWindowSize(window, &iw, &ih);
	return (float)iw / (float)ih;
}

glm::vec3 calcCamPos(GLFWwindow *window) {
	// calculate view & projection matrix
	static float angle = 45.0f,
		angle_vel = 0.0f,
		angle_acc = 0.3f,
		cam_dist = 4.5f,
		cam_y = 2.75f;
	
	angle_vel *= 0.9f;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		angle_vel += angle_acc;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		angle_vel -= angle_acc;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cam_dist -= 0.1f;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cam_dist += 0.1f;
	angle += angle_vel;
	if (cam_dist < 0.05f) cam_dist = 0.05f;
	
	glm::vec3 campos = glm::vec3(glm::cos(glm::radians(angle)) * cam_dist,
		cam_y, glm::sin(glm::radians(angle)) * cam_dist);
	
	return campos;
}

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
		DragFloat3("velocity", &vel[0], 0.001f);
		static glm::vec3 impulse;
		DragFloat3("  impulse", &impulse[0], 0.001f);
		SameLine();
		if (Button("Apply")) {
			vel = impulse;
		}
		SameLine();
		if (Button("X##2")) {
			registry.remove<CVelocity>(entity);
		}
	}
	if (registry.has<CBillboard>(entity)) {
		glm::vec2 &size = registry.get<CBillboard>(entity).size;
		glm::vec3 &color = registry.get<CBillboard>(entity).color;
		DragFloat2("size", &size[0], 0.001f);
		ColorPicker3("tint", &color[0]);
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
		
		DragFloat("radius", &radius, 0.0005f);
		DragFloat("force", &force, 0.0005f);
		
		if (Button("X##5")) {
			registry.remove<CSphereCollider>(entity);
		}
	}
}

void imguiEntitySpawn(entt::registry &registry) {
	using namespace ImGui;
	
	static glm::vec3 pos(0.0f);
	static glm::vec3 vel(0.0f);
	static glm::vec2 bbsize(0.1f, 0.07f);
	static glm::vec3 bbcolor(1.0f);
	static float rttforce = 0.01f;
	static glm::vec3 rttpos(0.0f);
	static float pressrad = 0.01f, pressforce = 0.01f;
	static float keycontrolspeed = 0.0015f;
	static int spawnamount = 1;
	static float spawnposoff = 0.05f,
				 spawnveloff = 0.02f;
	
	static bool haspos = false,
				hasvel = false,
				hasgrav = false,
				hasbb = false,
				hasruntt = false,
				haspresser = false,
				haskeyboard = false;
	
	if (Begin("spawn entity")) {
		Checkbox("CPosition", &haspos);
		Checkbox("CVelocity", &hasvel);
		Checkbox("CGravity", &hasgrav);
		Checkbox("CBillboard", &hasbb);
		Checkbox("CRunToTarget", &hasruntt);
		Checkbox("CSphereCollider", &haspresser);
		Checkbox("CKeyboardController", &haskeyboard);
		
		if (haspos) DragFloat3("position", &pos[0], 0.001f);
		if (hasvel) SliderFloat3("velocity", &vel[0], -0.1f, 0.1f);
		if (hasgrav) Text("Gravity: enabled");
		if (hasbb) {
			SliderFloat2("bb size", &bbsize[0], 0.001f, 0.1f);
			ColorEdit3("bb color", &bbcolor[0]);
		}
		if (hasruntt) {
			DragFloat3("rtt target", &rttpos[0], 0.001f);
			SliderFloat("rtt speed", &rttforce, 0.0f, 0.01f);
		}
		if (haspresser) {
			DragFloat("press radius", &pressrad, 0.0f, 0.1f);
			DragFloat("press force", &pressforce, 0.0f, 0.05f);
		}
		if (haskeyboard) {
			DragFloat("control speed", &keycontrolspeed, 0.0005f, 0.0035f);
		}
		if (Button("Spawn")) {
			for (int i = 0; i < spawnamount; ++i) {
				auto entity = registry.create();
				if (haspos) registry.assign<CPosition>(entity,
					m3d::randomizeVec3(pos, spawnposoff));
				if (hasvel) registry.assign<CVelocity>(entity,
					m3d::randomizeVec3(vel, spawnveloff));
				if (hasgrav) registry.assign<CGravity>(entity);
				if (hasbb) registry.assign<CBillboard>(entity, bbsize, bbcolor);
				if (hasruntt) registry.assign<CRunningToTarget>(entity, rttpos, rttforce);
				if (haspresser) registry.assign<CSphereCollider>(entity, pressrad, pressforce);
				if (haskeyboard) registry.assign<CKeyboardControllable>(entity, keycontrolspeed);
			}
		}
		SameLine();
		SliderInt("Amount", &spawnamount, 1, 50);
		SliderFloat("Position Offset", &spawnposoff, 0.0f, 0.5f);
		SliderFloat("Velocity Offset", &spawnveloff, 0.0f, 0.3f);
		
	}
	End();
}

GLFWwindow *window = nullptr;
int main(int, char**) {
	
	// disable buffering for stdout (fixes sublime text console)
	#if CFG_DEBUG
		setbuf(stdout, nullptr);
	#endif
	
	// init gl and window
	if (!initGL()) fprintf(stderr, "initGL() failed.\n");
	if (!initWindow(&window, 930, 640)) fprintf(stderr, "initWindow() failed.\n");
	
	// init game
	
	World world;
	
	AssetManager &assetManager = world.getAssetManager();
	assetManager.getTexture("res/images/sprites/default_soldier.png");
	assetManager.getTexture("res/images/sprites/default_soldier_e.png");
	assetManager.getTexture("res/images/sprites/default_soldier_s.png");
	assetManager.getTexture("res/images/sprites/default_soldier_se.png");
	assetManager.getTexture("res/images/sprites/default_soldier_sw.png");
	assetManager.getTexture("res/images/sprites/default_soldier_w.png");
	
	/* draw loop */
	double msLastTime = glfwGetTime();
	int msFrames = 0;
	while (!glfwWindowShouldClose(window)) {
		// poll events
		glfwPollEvents();
		imguiBeforeFrame();
		
		// calc time
		double msCurrentTime = glfwGetTime();
		static float msPerFrame = 0.0f;
		msFrames++;
		if (msCurrentTime - msLastTime >= 1.0) {
			msPerFrame = 1000.0f / (float)msFrames;
			msFrames = 0;
			msLastTime += 1.0;
		}
		
		#if CFG_IMGUI_ENABLED
			if (ImGui::Begin("performance")) {
				ImGui::Text("%g ms / frame", msPerFrame);
			}
			ImGui::End();
		#endif
		
		// input
		double mouseX, mouseY;
		glfwGetCursorPos(window, &mouseX, &mouseY);
		int screenX, screenY;
		glfwGetWindowSize(window, &screenX, &screenY);
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);
	
		// orbit camera calculations
		static glm::vec3 camtarget(0.0f);
		const float camToTargetSpeed = 0.08f;
		// ease camera to player or origin
		glm::vec3 targetPos;
		if (world.getPlayer() != entt::null && world.getRegistry().has<CPosition>(world.getPlayer())) {
			targetPos = world.getRegistry().get<CPosition>(world.getPlayer()).pos;
		} else { targetPos = glm::vec3(0.0f); }
		glm::vec3 toTarget = (targetPos - camtarget) * camToTargetSpeed;
		camtarget += toTarget;
		
		glm::vec3 campos = targetPos + calcCamPos(window);
		glm::mat4 uView = glm::lookAt(campos,
			camtarget, glm::vec3(0.0f, 0.5f, 0.0f));
		glm::mat4 uProj = glm::perspective(glm::radians(30.0f),
			getWindowAspectRatio(window), 0.1f, 100.0f);
		
		// rendering
		static FastNoise noise;
		noise.SetNoiseType(FastNoise::Perlin);
		
		float bgoff = noise.GetNoise(glfwGetTime() * 80.0f, 0.0f) * 0.03f;
		float bgoffr = (noise.GetNoise(glfwGetTime() * 90.0f, 0.8f) * 0.5f + 0.5f) * 0.04f;
		glClearColor(0.231f + bgoffr, 0.275f + bgoff, 0.302f + bgoff, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		#if CFG_IMGUI_ENABLED
			if (ImGui::Begin("world")) {
				static glm::vec3 pos(0.0f, 0.2f, 0.0f);
				static int amount = 50;
				static float spawnvel = 0.012f;
				static Random rng(-1.0f, 1.0f);
				ImGui::SliderFloat3("spawnpoint", &pos[0], -0.5f, 0.5f);
				ImGui::SliderInt("amount", &amount, 1, 500);
				ImGui::SliderFloat("velocity", &spawnvel, 0.0f, 0.2f);
				if (ImGui::Button("Spawn")) {
					for (int i = 0; i < amount; ++i) {
						entt::entity e = world.spawnDefaultEntity(pos);
						auto &vel = world.getRegistry().get<CVelocity>(e);
						vel.vel.x = rng();
						vel.vel.y = rng();
						vel.vel.z = rng();
						vel.vel = glm::normalize(vel.vel) * spawnvel;
					}
				}
				ImGui::Separator();
				
				auto gmp = m3d::raycastPlaneXZ(campos, glm::normalize(camtarget - campos),
					glm::vec2((float)mouseX, (float)mouseY),
					glm::vec2(1.0f * screenX, 1.0f * screenY), 0.0f);
				ImGui::Text("mouse: (%g, %g, %g)", gmp.x, gmp.y, gmp.z);
				
				static bool pickingMode = false;
				ImGui::Checkbox("entity picker", &pickingMode);
				static entt::entity selected = entt::null;
				if (pickingMode && glfwGetMouseButton(window, 0) == GLFW_PRESS) {
					selected = world.getNearestEntity(gmp);	
					pickingMode = false;
				}
				
				// entity editor
				imguiEntityEdit(world.getRegistry(), selected);
			}
			ImGui::End();
			imguiEntitySpawn(world.getRegistry());
		#endif
		
		world.update(campos, glm::normalize(campos - camtarget));
		world.draw(campos, uView, uProj);
		
		// present rendered
		imguiRender();
		glfwSwapBuffers(window);
	}

	/* cleanup */
	glfwDestroyWindow(window);
	glfwTerminate();
	imguiDestroy();
	printf("\ncleanup complete, quitting now...\n\n");
	return 0;
}
