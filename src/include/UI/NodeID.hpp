#pragma once

#include <string>

class NodeID {
public:

	NodeID();
	NodeID(const std::string &id);

	const std::string &get_id() const { return id; }

private:

	const std::string id;

};
