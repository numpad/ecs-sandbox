#pragma once

struct CSphereCollider {
	float radius, force;
	
	CSphereCollider()
		: radius(0.001f), force(0.001f)
	{
	}

	CSphereCollider(float radius, float force)
		: radius(radius), force(force)
	{
	}
	
};
