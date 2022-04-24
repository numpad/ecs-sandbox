#pragma once

#include <string>

class IWidget {
private:
	
	const std::string m_name;

public:

	IWidget(const std::string name) : m_name{name} {
		
	}

	virtual ~IWidget() {}
};
