#include <Window/Window.hpp>

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

	std::string source_string = "";
    switch (source) {
        case GL_DEBUG_SOURCE_API:             source_string = "API"; break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   source_string = "Window System"; break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER: source_string = "Shader Compiler"; break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:     source_string = "Third Party"; break;
        case GL_DEBUG_SOURCE_APPLICATION:     source_string = "Application"; break;
        case GL_DEBUG_SOURCE_OTHER:           source_string = "Other"; break;
    }

	fmt::text_style style;
    switch (type) {
        case GL_DEBUG_TYPE_ERROR:               style = fmt::fg(fmt::terminal_color::red); break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: style = fmt::fg(fmt::terminal_color::yellow); break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  style = fmt::fg(fmt::terminal_color::red); break; 
        case GL_DEBUG_TYPE_PORTABILITY:         style = fmt::fg(fmt::terminal_color::yellow); break;
        case GL_DEBUG_TYPE_PERFORMANCE:         style = fmt::fg(fmt::terminal_color::yellow); break;
        case GL_DEBUG_TYPE_MARKER:              style = fmt::fg(fmt::terminal_color::blue); break;
        case GL_DEBUG_TYPE_PUSH_GROUP:          style = fmt::fg(fmt::terminal_color::white); break;
        case GL_DEBUG_TYPE_POP_GROUP:           style = fmt::fg(fmt::terminal_color::white); break;
        case GL_DEBUG_TYPE_OTHER:               style = fmt::fg(fmt::terminal_color::white); break;
    }
    
	std::string severity_string = "";
    switch (severity)
    {
        case GL_DEBUG_SEVERITY_HIGH:         severity_string = "high"; break;
        case GL_DEBUG_SEVERITY_MEDIUM:       severity_string = "medium"; break;
        case GL_DEBUG_SEVERITY_LOW:          severity_string = "low"; break;
        case GL_DEBUG_SEVERITY_NOTIFICATION: severity_string = "info"; break;
    }

	fmt::print(style | fmt::emphasis::bold, "[{}] OpenGL error #{} from [{}]:\n", severity_string, id, source_string);
	fmt::print(style, "{}\n", message);
}

////////////
// STATIC //
////////////

bool Window::Init(int glmajor, int glminor) {
	// glfw init hints
	glfwInitHint(GLFW_JOYSTICK_HAT_BUTTONS, GLFW_FALSE);

	if (!glfwInit()) {	
		return false;
	}

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

	if (gl3wInit()) {
		fmt::print(fmt::fg(fmt::terminal_color::red), "gl3wInit() failed.\n");
		return false;
	}
	fmt::print(fmt::fg(fmt::terminal_color::green), "OpenGL {} with GLSL {}\n", glGetString(GL_VERSION), glGetString(GL_SHADING_LANGUAGE_VERSION));
	
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
		fmt::print(fmt::fg(fmt::terminal_color::yellow), "OpenGL debug context enabled\n");
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
