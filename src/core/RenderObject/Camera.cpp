#include <RenderObject/Camera.hpp>

////////////
// STATIC //
////////////

std::vector<Camera*> Camera::CAMERAS{};
GLFWwindow* Camera::MAIN_WINDOW = nullptr;

void Camera::Init(GLFWwindow* window) {
	Camera::MAIN_WINDOW = window;

	for (Camera* cam : Camera::CAMERAS) {
		int w, h;
		glfwGetWindowSize(window, &w, &h);

		cam->setScreenSize(w, h);
	}
}

////////////
// PUBLIC //
////////////

Camera::Camera(vec3 pos, float fov) : targetpos(pos + vec3(0.f, 0.f, 1.f)) {
	setFoV(fov);
	setPos(pos);
	CAMERAS.push_back(this);

	int w, h;
	glfwGetWindowSize(Camera::MAIN_WINDOW, &w, &h);
	setScreenSize(w, h);
	windowAspectLocked = true;
}

Camera::~Camera() {
	auto it = std::find(CAMERAS.begin(), CAMERAS.end(), this);
	if (it != CAMERAS.end()) {
		CAMERAS.erase(it);
		DEBUG(printf("[LOG] Camera: destructor called...\n"));
	} else
		DEBUGB {
			printf("[WRN] Camera: could not find own reference in destructor\n");
		}
}

void Camera::setPos(vec3 pos) {
	this->position = pos;
	updateView();
	updateToTarget();
}

void Camera::setTarget(vec3 target) {
	this->targetpos = target;
	updateView();
	updateToTarget();
}

void Camera::setScreenSize(int w, int h) {
	windowAspectLocked = false;
	screen_width = w;
	screen_height = h;
	aspect = float(screen_width) / float(screen_height);
	updateHudProj();
	updateProj();
}

void Camera::setFoV(float fov_angles) {
	fov = radians(fov_angles);
	updateProj();
}

const mat4& Camera::getProjection() const {
	return projection;
}

const mat4& Camera::getHudProjection() const {
	return hudProjection;
}

const mat4& Camera::getView() const {
	return this->view;
}

vec2 Camera::worldToScreen(vec3 worldpos) const {
	vec4 screen3d = projection * view * vec4(worldpos, 1.f);
	vec2 screen2d = vec2(screen3d.x, screen3d.y) / screen3d.z;
	return (screen2d * .5f + .5f) * vec2(float(screen_width), float(screen_height));
}

m3d::ray Camera::raycast(vec2 normalized_screenpos) const {
	vec3 raydir = m3d::mouseToCameraRay(getProjection(), getView(), normalized_screenpos);
	return m3d::ray(position, raydir);
}

/////////////
// PRIVATE //
/////////////

void Camera::updateHudProj() {
	hudProjection = ortho(0.f, float(screen_width), 0.f, float(screen_height));
}

void Camera::updateProj() {
	projection = perspective(fov, aspect, znear, zfar);

	// For orthographic projection:
	// float w = screen_width;
	// float h = screen_height;
	// float factor = 2.f;
	// float aspect = (w/h) * factor;
	// projection = ortho(-aspect, aspect, -factor, factor, znear, zfar);
}

void Camera::updateView() {
	view = lookAt(position, targetpos, world_up);
}

void Camera::updateToTarget() {
	this->dirToTarget = normalize(targetpos - position);
}
