#include "UI/Layout.hpp"
#include <fmt/color.h>
#include <fmt/core.h>

////////////
// STATIC //
////////////

////////////
// PUBLIC //
////////////

Layout::Layout() {
}

void Layout::draw() const {
	glm::mat3 view = glm::mat3(1.0f);

	drawChildren(m_layout, view);
}

void Layout::setLayout(YGNode* layout) {
	m_layout = layout;
}

void Layout::setWidget(std::string name, IWidget* widget) {
	m_widgetMapping[name] = widget;
}

/////////////
// PRIVATE //
/////////////

void Layout::drawChildren(YGNode* node, glm::mat3 view) const {
	float x = YGNodeLayoutGetLeft(node);
	float y = YGNodeLayoutGetTop(node);
	float w = YGNodeLayoutGetWidth(node);
	float h = YGNodeLayoutGetHeight(node);

	view = glm::translate(view, glm::vec2(x, y));
	glm::mat3 model = glm::scale(glm::mat3(1.0f), glm::vec2(w, h));
	
	NodeID* nodeId = (NodeID*)YGNodeGetContext(node);
	if (nodeId != nullptr && m_widgetMapping.find(nodeId->id) != m_widgetMapping.end()) {
		m_widgetMapping.at(nodeId->id)->draw(view * model);
	}

	for (uint32_t i = 0; i < YGNodeGetChildCount(node); ++i) {
		YGNodeRef child = YGNodeGetChild(node, i);
		drawChildren(child, view);
	}
}
