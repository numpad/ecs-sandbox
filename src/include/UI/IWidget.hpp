#pragma once

#include <string>
#include <glm/glm.hpp>

class IWidget {
private:
public:
	virtual ~IWidget() {
	}

	virtual void draw(const glm::mat3& transform) const = 0;
};
