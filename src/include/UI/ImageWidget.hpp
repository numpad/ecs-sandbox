#pragma once

#include <GL/gl3w.h>
#include <glm/glm.hpp>

#include "UI/IWidget.hpp"

class ImageWidget : public IWidget {
private:
	
	GLuint m_texture;

public:
	
	ImageWidget(GLuint texture);
	~ImageWidget();

	void draw(const glm::mat3 &transform) const;
};
