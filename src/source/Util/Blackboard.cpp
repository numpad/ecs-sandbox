#include <Util/Blackboard.hpp>

std::map<std::string, void*> Blackboard::storage{};

void Blackboard::write(std::string name, void* ptr) {
	Blackboard::storage.insert(std::make_pair(name, ptr));
}
