#pragma once

struct CJumpTimer {
	int timeUntilJump;

	CJumpTimer(int time = -1) : timeUntilJump(time) {
	}
};
