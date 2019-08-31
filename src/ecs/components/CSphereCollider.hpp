#pragma once

struct CSphereCollider {
	float radius, force;
	
	CSphereCollider(float radius, float force)
		: radius(radius), force(force)
	{
	}
	
};
