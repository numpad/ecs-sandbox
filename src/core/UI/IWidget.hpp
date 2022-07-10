#pragma once

#include <string>
#include <glm/glm.hpp>

class IWidget {
public:
	virtual ~IWidget() {
	}

	virtual void draw(const glm::mat3& transform) const = 0;

private:
};
