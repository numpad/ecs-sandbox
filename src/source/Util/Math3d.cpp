#include <Util/Math3d.hpp>

using namespace glm;

namespace m3d {

	vec3 randomizeVec3(vec3 v, float change) {
		static Random rand(-1.0f, 1.0f);
		return v + vec3(rand(), rand(), rand()) * change;
	}
	
	vec3 mouseToCameraRay(const mat4 &uProjection,
		const mat4 &uView, const vec2 ndcPoint)
	{
		using namespace glm;
		
		vec4 rayClip = vec4(ndcPoint.x, -ndcPoint.y, -1.0f, 1.0f);
		
		vec4 rayView = inverse(uProjection) * rayClip;
		rayView = vec4(rayView.x, rayView.y, -1.0f, 0.0f);
		
		vec3 rayWorld = vec3(inverse(uView) * rayView);
		rayWorld = normalize(rayWorld);
		
		return rayWorld;
	}
	
	vec3 raycast(ray ray, plane plane) {
		float t = -(dot(ray.origin, plane.normal) + plane.dist) / dot(ray.dir, plane.normal);
		return ray.origin + ray.dir * t;
	}
	
	float raycast(ISignedDistanceFunction &sdf, vec3 origin, vec3 dir, float max_length) {
		// if `dir` is not set, just test the `origin`
		if (length(dir) == 0.f) max_length = 0.f;
		else dir = normalize(dir);

		float nearest; // distance to the nearest point of the terrain (result of SDF)
		float dist; // distance we traveled on the ray
		int iteration = 0;
		constexpr int max_iterations = 100;
		for (dist = 0.f; dist <= max_length && iteration < max_iterations; dist += nearest) {
			vec3 p = origin + dir * dist;
			nearest = sdf.get_distance(p);
			if (nearest <= 0.f) {
				return dist;
			}
			// prevent infinite loop
			if (max_length == 0.f) return -1.f;
			iteration++;
		}

		// if we didn't exactly reach `max_length`, test it too.
		if (dist < max_length) {
			if (sdf.get_distance(origin + dir * max_length) <= 0.f) return max_length;
		}
		return -1.0f;
	}

	imat2x3 get_affected_chunks(mat2x3 aabb, vec3 chunksize) {
		return imat2x3(
			ivec3(floor(aabb[0] / chunksize)),
			ivec3(ceil(aabb[1] / chunksize)) - 1
		);
	}

	int solve_ballistic_arc(vec3 proj_pos, float proj_speed, vec3 target, float gravity, vec3 &s0, vec3 &s1) {

        // Handling these cases is up to your project's coding standards
        assert(proj_pos != target && proj_speed > 0.0f && gravity > 0.0f);

        // C# requires out variables be set
        s0 = vec3();
        s1 = vec3();

        // Derivation
        //   (1) x = v*t*cos O
        //   (2) y = v*t*sin O - .5*g*t^2
        // 
        //   (3) t = x/(cos O*v)                                        [solve t from (1)]
        //   (4) y = v*x*sin O/(cos O * v) - .5*g*x^2/(cos^2 O*v^2)     [plug t into y=...]
        //   (5) y = x*tan O - g*x^2/(2*v^2*cos^2 O)                    [reduce; cos/sin = tan]
        //   (6) y = x*tan O - (g*x^2/(2*v^2))*(1+tan^2 O)              [reduce; 1+tan O = 1/cos^2 O]
        //   (7) 0 = ((-g*x^2)/(2*v^2))*tan^2 O + x*tan O - (g*x^2)/(2*v^2) - y    [re-arrange]
        //   Quadratic! a*p^2 + b*p + c where p = tan O
        //
        //   (8) let gxv = -g*x*x/(2*v*v)
        //   (9) p = (-x +- sqrt(x*x - 4gxv*(gxv - y)))/2*gxv           [quadratic formula]
        //   (10) p = (v^2 +- sqrt(v^4 - g(g*x^2 + 2*y*v^2)))/gx        [multiply top/bottom by -2*v*v/x; move 4*v^4/x^2 into root]
        //   (11) O = atan(p)

        vec3 diff = target - proj_pos;
        vec3 diffXZ = vec3(diff.x, 0.0f, diff.z);
        float groundDist = length(diffXZ);

        float speed2 = proj_speed*proj_speed;
        float speed4 = proj_speed*proj_speed*proj_speed*proj_speed;
        float y = diff.y;
        float x = groundDist;
        float gx = gravity*x;

        float root = speed4 - gravity*(gravity*x*x + 2.0f*y*speed2);

        // No solution
        if (root < 0.0f)
            return 0;

        root = sqrt(root);

        float lowAng = atan2(speed2 - root, gx);
        float highAng = atan2(speed2 + root, gx);
        int numSolutions = lowAng != highAng ? 2 : 1;
        
        vec3 groundDir = normalize(diffXZ);
        vec3 UP(0.0f, 1.0f, 0.0f);
        s0 = groundDir*cos(lowAng)*proj_speed + UP*sin(lowAng)*proj_speed;
        if (numSolutions > 1)
            s1 = groundDir*cos(highAng)*proj_speed + UP*sin(highAng)*proj_speed;

        return numSolutions;
    }
    
    float ballistic_range(float speed, float gravity, float initial_height) {

		// Handling these cases is up to your project's coding standards
		assert(speed > 0.0f && gravity > 0.0f && initial_height >= 0.0f);

		// Derivation
		//   (1) x = speed * time * cos O
		//   (2) y = initial_height + (speed * time * sin O) - (.5 * gravity*time*time)
		//   (3) via quadratic: t = (speed*sin O)/gravity + sqrt(speed*speed*sin O + 2*gravity*initial_height)/gravity	[ignore smaller root]
		//   (4) solution: range = x = (speed*cos O)/gravity * sqrt(speed*speed*sin O + 2*gravity*initial_height)	[plug t back into x=speed*time*cos O]
		float angle = radians(45.0f); // no air resistence, so 45 degrees provides maximum range
		float cosp = cos(angle);
		float sinp = sin(angle);

		float range = (speed*cosp/gravity) * (speed*sinp + sqrt(speed*speed*sinp*sinp + 2.0f*gravity*initial_height));
		return range;
	}
	
	bool solve_ballistic_arc_lateral(vec3 proj_pos, float lateral_speed, vec3 target_pos, float max_height, vec3 &fire_velocity, float &gravity) {
	
		// Handling these cases is up to your project's coding standards
		assert(proj_pos != target_pos && lateral_speed > 0 && max_height > proj_pos.y);

		fire_velocity = vec3();
		gravity = 0.0f;

		vec3 diff = target_pos - proj_pos;
		vec3 diffXZ = vec3(diff.x, 0.0f, diff.z);
		float lateralDist = length(diffXZ);

		if (lateralDist == 0.0f)
			return false;

		float time = lateralDist / lateral_speed;

		fire_velocity = normalize(diffXZ) * lateral_speed;

		// System of equations. Hit max_height at t=.5*time. Hit target at t=time.
		//
		// peak = y0 + vertical_speed*halfTime + .5*gravity*halfTime^2
		// end = y0 + vertical_speed*time + .5*gravity*time^s
		// Wolfram Alpha: solve b = a + .5*v*t + .5*g*(.5*t)^2, c = a + vt + .5*g*t^2 for g, v
		float a = proj_pos.y;	   // initial
		float b = max_height;	   // peak
		float c = target_pos.y;	 // final

		gravity = -4.0f*(a - 2*b + c) / (time* time);
		fire_velocity.y = (-(3*a - 4*b + c) / time);

		return true;
	}
	
	vec3 triangleCalcNormal(vec3 *triangle) {
		vec3 a = triangle[0],
		     b = triangle[1],
		     c = triangle[2];

		vec3 ba = (a - b),
		     ca = (a - c);

		return normalize(cross(ba, ca));
	}
}
