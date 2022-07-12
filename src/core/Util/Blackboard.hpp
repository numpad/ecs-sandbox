#pragma once

#include <map>
#include <string>

class Blackboard {
public:
	static void write(std::string name, void* ptr);

	template <class T>
	static T* read(std::string name) {
		return (T*)Blackboard::storage[name];
	}

private:
	static std::map<std::string, void*> storage;
};
