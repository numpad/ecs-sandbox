#pragma once

#include <string>
#include <glm/glm.hpp>

class IWidget {
public:
	virtual ~IWidget() {
	}

	virtual void draw(const glm::mat3& view, const glm::mat3& model) const = 0;

private:
};
