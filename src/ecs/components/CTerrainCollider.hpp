#pragma once

struct CTerrainCollider {
	bool is_grounded;
	float dist_to_dir;
	float stair_height;
	float max_stair_height = 0.04f;
};
