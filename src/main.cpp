#include "config.hpp"
#include <GL/gl3w.h>

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <thread>
#include <string>

#include <GLFW/glfw3.h>
#include <FastNoise/FastNoise.h>
#include <stb/stb_image.h>
#include <util/sgl_shader.hpp>
#include <util/sgl_framebuffer.hpp>
#include <util/sgl_texture.hpp>
#include <util/sgl_renderbuffer.hpp>
#include <util/sgl_attachment.hpp>
#include <util/sgl_audio_listener.hpp>
#include <util/sgl_audio_source.hpp>
#include <util/sgl_audio.hpp>
#include <dr/dr_wav.h>
#include <imgui/imgui.h>
#include <imgui/examples/imgui_impl_glfw.h>
#include <imgui/examples/imgui_impl_opengl3.h>
#include <entt/entt.hpp>

#include <Window/Window.hpp>

#include <Assets/Texture.hpp>
#include <Assets/AssetManager.hpp>
#include <ecs/components.hpp>
#include <ecs/systems.hpp>
#include <ecs/events.hpp>
#include <World.hpp>

#include <Terrain/SignedDistTerrain.hpp>
#include <Terrain/ChunkedTerrain.hpp>
#include <Terrain/CubeMarcher.hpp>

#include <RenderObject/Camera.hpp>
#include <RenderObject/ChunkedWorld.hpp>

#include <cereal/types/memory.hpp>
#include <cereal/archives/json.hpp>

#include <Util/Math3d.hpp>
#include <Util/Font.hpp>
#include <Util/Random.hpp>
#include <Util/Benchmark.hpp>
#include <Util/Blackboard.hpp>

#include <Script/ScriptBinder.hpp>

#if CFG_DEBUG
	#include <Debug/ImguiPresets.hpp>
#endif

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
		printf("[INIT] dear imgui %s\n", ImGui::GetVersion());
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

float getWindowAspectRatio(GLFWwindow *window) {
	int iw, ih;
	glfwGetWindowSize(window, &iw, &ih);
	return (float)iw / (float)ih;
}

glm::vec3 calcCamPos(GLFWwindow *window) {
	// calculate view & projection matrix
	static float angle = 0.0f,
		angle_vel = 0.0f,
		angle_acc = 0.3f,
		cam_dist = 4.5f,
		cam_y = 3.2f;
	
	angle_vel *= 0.9f;
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		angle_vel += angle_acc;
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		angle_vel -= angle_acc;
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		cam_dist -= 0.1f;
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		cam_dist += 0.1f;
	angle += angle_vel;
	if (cam_dist < 0.05f) cam_dist = 0.05f;
	
	glm::vec3 campos = glm::vec3(glm::cos(glm::radians(angle)) * cam_dist,
		cam_y, glm::sin(glm::radians(angle)) * cam_dist);
	
	return campos;
}

int main(int, char**) {
	// disable buffering for stdout (fixes sublime text console)
	#if CFG_DEBUG
		setbuf(stdout, nullptr);
	#endif
	
	// init gl and window
	Window window;
	if (!Window::Init()) std::cerr << "[INIT] Window::Init() failed." << std::endl;
	if (!window.create(930, 640)) std::cerr << "[INIT] window.create() failed." << std::endl;
	imguiInit(window);

	// init game
	Camera::Init(window);
	Font::Init();
	
	std::shared_ptr<Camera> camera = std::make_shared<Camera>(vec3(0.f));
	std::shared_ptr<Camera> topdown = std::make_shared<Camera>(vec3(7.f, 5.f, -4.f));
	topdown->setTarget(vec3(0.f, 0.f, 0.f));
	World world(window, camera);
	
	world.load();
	
	//AssetManager &assetManager = world.getAssetManager();
	
	Font defaultFont("res/fonts/FSmono.ttf", 48);
	
	
	//ScriptBinder::luaTest();
	
	// deferred rendering
	sgl::texture color_buffer, position_buffer, normal_buffer;
	color_buffer.load(camera->getScreenWidth(), camera->getScreenHeight(), sgl::texture::internalformat::rgb, nullptr, sgl::texture::format::rgba, sgl::texture::datatype::u8);
	position_buffer.load(camera->getScreenWidth(), camera->getScreenHeight(), sgl::texture::internalformat::rgba16f);
	position_buffer.set_filter(sgl::texture::filter::nearest);
	normal_buffer.load(camera->getScreenWidth(), camera->getScreenHeight(), sgl::texture::internalformat::rgba16f);
	normal_buffer.set_filter(sgl::texture::filter::nearest);
	
	sgl::renderbuffer depthstencil_buffer;
	depthstencil_buffer.create(camera->getScreenWidth(), camera->getScreenHeight(), sgl::renderbuffer::internalformat::depth24_stencil8);
	
	// sgl::texture depthstencil_buffer;
	//depthstencil_buffer.load(camera->getScreenWidth(), camera->getScreenHeight(),
	//	sgl::texture::internalformat::depth24_stencil8, nullptr,
	//	sgl::texture::format::depth_stencil, sgl::texture::datatype::u24_8);

	sgl::framebuffer screen_fbo;
	screen_fbo.attach(color_buffer, sgl::attachment::color(0));
	screen_fbo.attach(position_buffer, sgl::attachment::color(1));
	screen_fbo.attach(normal_buffer, sgl::attachment::color(2));
	screen_fbo.attach(depthstencil_buffer, sgl::attachment::depth_stencil());
	screen_fbo.targets();
	
	// build screen mesh
	GLfloat vertices[] = {
		-1.f, -1.f,
		 1.f, -1.f,
		-1.f,  1.f,
		-1.f,  1.f,
		 1.f, -1.f,
		 1.f,  1.f,
		 
		 0.f, 0.f,
		 1.f, 0.f,
		 0.f, 1.f,
		 0.f, 1.f,
		 1.f, 0.f,
		 1.f, 1.f
	};
	GLuint svao, svbo;
	glGenBuffers(1, &svbo);
	glGenVertexArrays(1, &svao);
	
	glBindVertexArray(svao);
	glBindBuffer(GL_ARRAY_BUFFER, svbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 24, vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void *)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void *)(sizeof(GLfloat) * 12));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	
	sgl::shader screen_shader;
	screen_shader.load("res/glsl/2d/screen_vert.glsl", sgl::shader::VERTEX);
	screen_shader.load("res/glsl/2d/screen_frag.glsl", sgl::shader::FRAGMENT);
	screen_shader.compile();
	screen_shader.link();
	Blackboard::write("deferredShader", &screen_shader);
	
	screen_shader["uTexColor"] = 0;
	screen_shader["uTexPosition"] = 1;
	screen_shader["uTexNormal"] = 2;
	
	/* audio test */
	auto audio_outputs = sgl::audio_listener::get_available();
	std::cout << "Listing available audio output devices." << std::endl;
	for (auto out : audio_outputs) {
		std::cout << " * " << out << std::endl;
	}
	
	sgl::audio_listener listener;
	sgl::audio_source source;
	sgl::audio audio;
	unsigned int channels;
	unsigned int sampleRate;
	drwav_uint64 totalPCMFrameCount;
	short *pSampleData = drwav_open_file_and_read_pcm_frames_s16("res/audio/sfx/hit.wav", &channels, &sampleRate, &totalPCMFrameCount, NULL);
	if (pSampleData == NULL) {
		std::cout << "[ERR] DRWAV: Could not read file." << std::endl;
	}
	
	audio.load(sgl::audio::format::mono16, sampleRate, totalPCMFrameCount * channels * sizeof(short), pSampleData);
	drwav_free(pSampleData, nullptr);
	source.set_buffer(audio);
	source.play();
	
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
		
		// input
		double mouseX, mouseY;
		glfwGetCursorPos(window, &mouseX, &mouseY);
		int screenX, screenY;
		glfwGetWindowSize(window, &screenX, &screenY);
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);
		glm::vec2 normalizedMouse = glm::vec2(mouseX / (float)screenX,
			mouseY / (float)screenY) * 2.0f - 1.0f;
		
		// orbit camera calculations
		static glm::vec3 camtarget(0.0f);
		const float camToTargetSpeed = 0.12f;
		// ease camera to player or origin
		glm::vec3 targetPos;
		if (world.getPlayer() != entt::null && world.getRegistry().has<CPosition>(world.getPlayer())) {
			targetPos = world.getRegistry().get<CPosition>(world.getPlayer()).pos * glm::vec3(1.0f, 0.0f, 1.0f);
		} else { targetPos = glm::vec3(0.0f); }
		glm::vec3 toTarget = (targetPos - camtarget) * camToTargetSpeed;
		camtarget += toTarget;
		
		glm::vec3 campos = targetPos + calcCamPos(window);
		camera->setPos(campos);
		camera->setTarget(camtarget);
				
		// calculate player aim
		entt::entity worldCrosshair = world.getCrosshair();
		
		m3d::ray mcRay = world.getCamera()->raycast(normalizedMouse);
		m3d::plane mcFloor(glm::vec3(0.0f, 1.0f, 0.0f));
		glm::vec3 crosspos = m3d::raycast(mcRay, mcFloor);
		bool mouseRightDown = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;

		if (world.getRegistry().valid(worldCrosshair)) {
			world.getRegistry().get<CPosition>(worldCrosshair).pos = crosspos;
		}
		
		#if CFG_IMGUI_ENABLED
			static int settings_attachment = GL_COLOR_ATTACHMENT2;
			imguiRenderMenuBar(window, world, crosspos, topdown, camera, msPerFrame, settings_attachment);
		#endif
		
		// TODO: find a better way to resize fbo attachments
		color_buffer.resize(camera->getScreenWidth(), camera->getScreenHeight());
		position_buffer.resize(camera->getScreenWidth(), camera->getScreenHeight());
		normal_buffer.resize(camera->getScreenWidth(), camera->getScreenHeight());
		depthstencil_buffer.create(camera->getScreenWidth(), camera->getScreenHeight(), sgl::renderbuffer::internalformat::depth24_stencil8);
		
		// rendering
		screen_fbo.bind();
		
		glClearColor(.231f, .275f, .302f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		// actual rendering
		if (world.is_loaded()) {
			world.update();
			world.draw();
		}
		screen_fbo.unbind();
		
		// render framebuffer to screen
		#if CFG_IMGUI_ENABLED
			screen_shader["uTexChoiceActive"] = (settings_attachment != 0);
			GLint display_texture = color_buffer;
			if (settings_attachment != 0) {
				glGetNamedFramebufferAttachmentParameteriv(screen_fbo, settings_attachment, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &display_texture);
			}
			screen_shader["uTexColor"] = 0;
			screen_shader["uTexPosition"] = 1;
			screen_shader["uTexNormal"] = 2;
		#else
			const GLint display_texture = color_buffer;
		#endif
		GLint pmode;
		glGetIntegerv(GL_POLYGON_MODE, &pmode);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		
		screen_shader.use();
		glDisable(GL_DEPTH_TEST);
		glBindVertexArray(svao);
		glBindBuffer(GL_ARRAY_BUFFER, svbo);
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, color_buffer);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, position_buffer);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, normal_buffer);
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, display_texture);
		
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
		glEnable(GL_DEPTH_TEST);
		glPolygonMode(GL_FRONT_AND_BACK, pmode);
		
		// present rendered
		imguiRender();
		glfwSwapBuffers(window);
		// poll events here?
	}

	/* cleanup */
	world.destroy();
	defaultFont.destroy();
	window.destroy();
	
	glDeleteBuffers(1, &svbo);
	glDeleteVertexArrays(1, &svao);
	
	Font::Destroy();
	Window::Destroy();
	imguiDestroy();
	std::cout << "[SYS]" << "cleanup complete, quitting now..." << std::endl;
	return 0;
}
