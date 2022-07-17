#pragma once

#include "Assets/Mesh.hpp"
#include "Assets/Vertex.hpp"
#include "Graphics/GLState.hpp"
#include "sgl/sgl_shader.hpp"
#include <par/par_shapes.h>
#include <fmt/core.h>
#include "Engine/Engine.hpp"

class OceanPlane {
public:
	OceanPlane() {
		par_shapes_mesh_s* plane = par_shapes_create_plane(m_planeSubdivisions, m_planeSubdivisions);
		const float _rotation[] = {1, 0, 0};
		par_shapes_rotate(plane, PAR_PI * -0.5f, _rotation);

		std::vector<Vertex> vertices;
		for (int i = 0; i < plane->npoints * 3; i += 3) {
			vertices.emplace_back(Vertex(glm::vec3(plane->points[i + 0], plane->points[i + 1], plane->points[i + 2])));
		}

		std::vector<GLuint> indices;
		for (int i = 0; i < plane->ntriangles * 3; ++i) {
			indices.push_back(plane->triangles[i]);
		}
		par_shapes_free_mesh(plane);

		m_mesh = new Mesh(vertices, indices, false);

		m_shader.load("res/glsl/raft/water_vert.glsl", sgl::shader::VERTEX);
		m_shader.load("res/glsl/raft/water_frag.glsl", sgl::shader::FRAGMENT);
		m_shader.compile();
		m_shader.link();
	}

	void draw(Camera& camera) {
		// additive blending
		GLState waterState;
		waterState.depth_test = true;
		Engine::Instance->getGraphics().setState(waterState);
		
		GBuffer& gbuffer = Engine::Instance->getGBuffer();

		glm::mat4 model = glm::translate(
			glm::scale(glm::mat4(1.0f), glm::vec3(m_size.x, 1.0f, m_size.y)),
			glm::vec3(-0.5f, 0.0f, 0.5f));
		m_shader["uProjection"] = camera.getProjection();
		m_shader["uView"] = camera.getView();
		m_shader["uModel"] = model;
		m_shader["uTime"] = (float)glfwGetTime();
		m_shader["uSize"] = m_size;
		m_shader["uBaseColor"] = m_baseColor;
		m_shader["uScreenResolution"] = glm::vec2(Engine::Instance->getWindow().getSize());
		m_shader["uCameraPosition"] = camera.getPos();
		m_shader["uDepthTexture"] = 0;
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gbuffer.m_depth->get_texture());
		m_shader["uPositionTexture"] = 1;
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, gbuffer.m_position->get_texture());

		m_mesh->draw(m_shader);

#ifndef NDEBUG
		static int _ticks = 0;
		if (_ticks++ > 50) {
			_ticks = 0;
			m_shader.reload();
		}
#endif
	}

	~OceanPlane() {
		delete m_mesh;
	}

private:
	Mesh* m_mesh;
	sgl::shader m_shader;
	glm::vec2 m_size = glm::vec2(6.0f);
	glm::vec3 m_baseColor = glm::vec3(0.40, 0.53, 0.78);
	const unsigned int m_planeSubdivisions = 18;
};
