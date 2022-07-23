#include <Assets/Vertex.hpp>

Vertex::Vertex(glm::vec3 pos, glm::vec3 normal, glm::vec2 uv) : position(pos), normal(normal), color(glm::vec3(1.0f)), texcoords(uv) {
}
