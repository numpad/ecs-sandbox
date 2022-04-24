#pragma once
#include <UI/IWidget.hpp>
#include <GL/gl3w.h>

class ImageWidget : public IWidget {
private:
	
	GLuint m_texture;

public:
	
	ImageWidget(const std::string name, GLuint texture);
	~ImageWidget();

};
