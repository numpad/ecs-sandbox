#include <set>
#include <glm/gtx/matrix_transform_2d.hpp>
#include "Graphics/GLState.hpp"
#include "UI/Layout.hpp"
#include "UI/IWidget.hpp"
#include "UI/NodeID.hpp"
#include "Engine/Engine.hpp"

////////////
// STATIC //
////////////

////////////
// PUBLIC //
////////////

Layout::Layout() {
}

void Layout::draw() const {
	if (m_layout == nullptr) {
		return;
	}

	GLState state;
	state.blend = true;
	Engine::Instance->graphics.setState(state);

	glm::mat3 view = glm::mat3(1.0f);
	drawChildren(m_layout, view);
}

void Layout::resize(glm::vec2 size) {
	if (m_layout == nullptr) {
		return;
	}

	m_size = size;
	YGNodeCalculateLayout(m_layout, size.x, size.y, YGDirectionLTR);
}

void Layout::setLayout(YGNodeRef layout) {
	m_layout = layout;
	resize(glm::vec2(Engine::Instance->window.getSize()));
}

YGNodeRef Layout::getLayout() const {
	return m_layout;
}

void Layout::setWidget(std::string name, IWidget* widget) {
	m_widgetMapping[name] = widget;
}

/**
 * @brief Removes and `delete`s all widgets.
 */
void Layout::deleteWidgets() {
	std::set<IWidget*> widgets;

	// collect all widgets, duplicates are discarded
	for (auto it = m_widgetMapping.begin(); it != m_widgetMapping.end(); ++it) {
		widgets.insert(it->second);
	}

	// delete widgets
	for (const IWidget* widget : widgets) {
		delete widget;
	}

	// clear widget mapping
	m_widgetMapping.clear();
}

/////////////
// PRIVATE //
/////////////

void Layout::drawChildren(YGNodeRef node, glm::mat3 view) const {
	float x = YGNodeLayoutGetLeft(node);
	float y = YGNodeLayoutGetTop(node);
	float w = YGNodeLayoutGetWidth(node);
	float h = YGNodeLayoutGetHeight(node);

	view = glm::translate(view, glm::vec2(x, y));
	glm::mat3 model = glm::scale(glm::mat3(1.0f), glm::vec2(w, h));

	NodeID* nodeId = (NodeID*)YGNodeGetContext(node);
	if (nodeId != nullptr) {
		if (m_widgetMapping.find(nodeId->id) != m_widgetMapping.end()) {
			m_widgetMapping.at(nodeId->id)->draw(view, model);
		} else if (m_widgetMapping.find("*") != m_widgetMapping.end()) {
			m_widgetMapping.at("*")->draw(view, model);
		}
	}

	for (uint32_t i = 0; i < YGNodeGetChildCount(node); ++i) {
		YGNodeRef child = YGNodeGetChild(node, i);
		drawChildren(child, view);
	}
}
