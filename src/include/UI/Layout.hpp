#pragma once

#include <string>
#include <unordered_map>

#include <yoga/Yoga.h>
#include <glm/glm.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>

#include "UI/IWidget.hpp"
#include "UI/NodeID.hpp"

typedef std::unordered_map<std::string, IWidget*> WidgetMap;

class Layout {
public:
	Layout();

	void draw() const;

	void setLayout(YGNode* layout);
	void setWidget(std::string name, IWidget* widget);

private:
	YGNode* m_layout = nullptr;
	WidgetMap m_widgetMapping;

	void drawChildren(YGNode* node, glm::mat3 view) const;
};
