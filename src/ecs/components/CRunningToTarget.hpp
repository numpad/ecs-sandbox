#pragma once

struct CRunningToTarget {
	glm::vec3 target;
	
	CRunningToTarget(glm::vec3 target) 
		: target(target)
	{
	}
};
