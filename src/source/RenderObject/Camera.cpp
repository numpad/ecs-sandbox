#include <RenderObject/Camera.hpp>

////////////
// STATIC //
////////////

std::vector<Camera *> Camera::CAMERAS{};
GLFWwindow *Camera::MAIN_WINDOW = nullptr;

void Camera::Init(GLFWwindow *window) {
	Camera::MAIN_WINDOW = window;
	
	for (Camera *cam : Camera::CAMERAS) {
		int w, h;
		glfwGetWindowSize(window, &w, &h);
		
		cam->setScreenSize(w, h);
	}
}


////////////
// PUBLIC //
////////////

Camera::Camera(vec3 pos, float fov)
	: fov(fov), pos(pos) {
	CAMERAS.push_back(this);
	
	int w, h;
	glfwGetWindowSize(Camera::MAIN_WINDOW, &w, &h);	
	setScreenSize(w, h);
	windowAspectLocked = true;
	
	DEBUGB {
		printf("[LOG] Camera: constructor\n");
	}
}

Camera::~Camera() {
	auto it = std::find(CAMERAS.begin(), CAMERAS.end(), this);
	if (it != CAMERAS.end()) {
		CAMERAS.erase(it);
	}
	else DEBUGB {
		printf("[WRN] Camera: could not find own reference in destructor\n");
	}
}

void Camera::setScreenSize(int w, int h) {
	windowAspectLocked = false;
	screen_width = w;
	screen_height = h;
	
	updateHudProj();
}

const mat4& Camera::getHudProjection() const {
	return hudProjection;
}


/////////////
// PRIVATE //
/////////////

void Camera::updateHudProj() {
	hudProjection = glm::ortho(0.f, float(screen_width), 0.f, float(screen_height));
}