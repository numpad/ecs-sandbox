#include <Window/Window.hpp>

static void onResize(GLFWwindow *, int width, int height) {
	glViewport(0, 0, width, height);
	for (Camera *cam : Camera::CAMERAS) {
		if (cam->windowAspectLocked) {
			cam->setScreenSize(width, height);
			cam->windowAspectLocked = true;
		}
	}
}

static void APIENTRY glDebugOutput(GLenum source,
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

////////////
// STATIC //
////////////

bool Window::Init(int glmajor, int glminor) {
	#if CFG_DEBUG
		const char *cfg_debug_state = "DEBUG";
	#else
		const char *cfg_debug_state = "Release build";
	#endif

	std::cout << "[INIT] " << CFG_PROJECT_NAME << " version: " << CFG_VERSION_MAJOR << "." << CFG_VERSION_MINOR << " (" << cfg_debug_state << ")" << std::endl;

	// glfw init hints
	glfwInitHint(GLFW_JOYSTICK_HAT_BUTTONS, GLFW_FALSE);

	if (!glfwInit()) {
		fprintf(stderr, "glfwInit() failed.\n");
		std::cerr << "[INIT] glfwInit() failed." << std::endl;
		return false;
	}

	std::cout << "[INIT] GLFW " << glfwGetVersionString() << std::endl;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, glmajor);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, glminor);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	#if CFG_DEBUG
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, (CFG_DEBUG ? GLFW_TRUE : GLFW_FALSE));
	#endif

	return true;
}

void Window::Destroy() {
	glfwTerminate();
}

////////////
// PUBLIC //
////////////

bool Window::create(int width, int height, FullscreenMode fullscreenMode) {
	GLFWmonitor *monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode *mode = glfwGetVideoMode(monitor);
	if (width < 0) width = mode->width;
	if (height < 0) height = mode->height;
	if (fullscreenMode == FullscreenMode::NONE) monitor = nullptr;

	glfw_window = glfwCreateWindow(width, height, "main", monitor, nullptr);
	glfwMakeContextCurrent(glfw_window);
	glfwSwapInterval(1);
	glfwSetFramebufferSizeCallback(glfw_window, onResize);

	if (gl3wInit()) {
		fprintf(stderr, "gl3wInit() failed.\n");
		return false;
	}
	printf("[INIT] OpenGL %s, GLSL %s\n",
		glGetString(GL_VERSION),
		glGetString(GL_SHADING_LANGUAGE_VERSION));
	
	// msaa
	//glEnable(GL_MULTISAMPLE);
	//glEnable(GL_SAMPLE_SHADING);
	//glMinSampleShading(1.0f);
	
	// depth test
	glEnable(GL_DEPTH_TEST);
	//glDepthFunc(GL_LEQUAL);
	
	// blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);
	
	// face culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	
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
	
	return true;
}

void Window::destroy() {
	glfwDestroyWindow(glfw_window);
}

glm::ivec2 Window::getMousePosition() const {
	double mouseX, mouseY;
	glfwGetCursorPos(glfw_window, &mouseX, &mouseY);
	return glm::ivec2((int)mouseX, (int)mouseY);
}

glm::vec2 Window::getNormalizedMousePosition() const {
	double mouseX, mouseY;
	glfwGetCursorPos(glfw_window, &mouseX, &mouseY);
	glm::ivec2 window_size = getSize();
	return glm::vec2(mouseX / (float)window_size.x, mouseY / (float)window_size.y) * 2.0f - 1.0f;
}

glm::ivec2 Window::getSize() const {
	int screenX, screenY;
	glfwGetWindowSize(glfw_window, &screenX, &screenY);
	return glm::ivec2(screenX, screenY);
}
