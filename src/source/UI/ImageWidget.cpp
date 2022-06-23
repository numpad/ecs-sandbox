#include <UI/ImageWidget.hpp>

ImageWidget::ImageWidget(GLuint texture)
 : m_texture{texture}
{

}

ImageWidget::~ImageWidget() {

}

void ImageWidget::draw(const glm::mat3 &transform) const {

}
