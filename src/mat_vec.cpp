#include "./mat_vec.h"

vec3 cross(const vec3& p, const vec3& v){
	return vec3{p.y*v.z - p.z*v.y, p.z*v.x - p.x*v.z, p.x*v.y - p.y*v.x};
}
