#pragma once

struct CHealth {
	int hp, max_hp;
	
	CHealth() : hp(1), max_hp(1) {}
	CHealth(int hp) : hp(hp), max_hp(hp) {}
	
};
