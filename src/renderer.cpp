#include "renderer.h"


// Applies the fn function to first three bytes (red, green, and blue)
// be wary of the rollover
std::uint32_t modify_channels(std::uint32_t color, std::uint8_t (*fn)(std::uint8_t)) {
	std::uint8_t* bytes = (std::uint8_t*)&color;
	for (int i = 0; i < 3; i++) {
		bytes[i] = fn(bytes[i]);
	}
	return *(std::uint8_t*)bytes;
}

// Multiply each channel separately by scale
// limits it from above to 255 in each channel
std::uint32_t color_mult(std::uint32_t color, double scale){
	assert(scale >= 0);
	std::uint32_t ret = 0xff000000;
	ret += std::min(((std::uint32_t)(scale*((color & 0x000000ff)>>(8*0))))<<(8*0), 255U<<0);
	ret += std::min(((std::uint32_t)(scale*((color & 0x0000ff00)>>(8*1))))<<(8*1), 255U<<8);
	ret += std::min(((std::uint32_t)(scale*((color & 0x00ff0000)>>(8*2))))<<(8*2), 255U<<16);
	return ret;
}

int clamp(int low, int val, int high) { return std::max(low, std::min(high, val)); }
float clampf(float low, float val, float high) {return std::max(low, std::min(high, val));}

mat<4,4> look_at(vec<3> eye, vec<3> center, vec<3> up) {
	vec<3> k = (eye-center).normalized();
	vec<3> i = cross(up, k).normalized();
	vec<3> j = cross(k, i).normalized();
	return mat<4,4>{
		i[0], i[1], i[2], (-1)*(center*i),
		j[0], j[1], j[2], (-1)*(center*j),
		k[0], k[1], k[2], (-1)*(center*k),
		0   , 0   , 0   ,   1,
	};
}

// Returns the matrix which bounds (-1,+1)*(-1,+1)*(-1,+1) to (x, y)*(x+w, y+h)*(0,d)
// -1 in the entry (1,1) needed to flip vertically
mat<4,4> get_viewport(int x, int y, int w, int h, int d) {
	return mat<4,4>{
		vec<4>{(double)w/2,               0,          0,x + (double)w/2},
		vec<4>{          0,(-1)*(double)h/2,          0,y + (double)h/2},
		vec<4>{          0,               0,(double)d/2,    (double)d/2},
		vec<4>{          0,               0,          0,              1},
	};
}

mat<4,4> get_projection(float c=1.f) {
	return mat<4,4>{
		vec<4>{1,0,  0, 0},
		vec<4>{0,1,  0, 0},
		vec<4>{0,0,  1, 0},
		vec<4>{0,0,-1/c,1},
	};
}

vec3 get_barycentric(std::array<vec3, 3> vertices, vec2i P){
	// Initializing the vector that will contain
	// barycentric coordinates
	vec3 ret = {};

	double det_T = (vertices[1].y - vertices[2].y)*(vertices[0].x - vertices[2].x) + (vertices[2].x - vertices[1].x)*(vertices[0].y - vertices[2].y);
	double T_inv[2][2] = {
		{(vertices[1].y - vertices[2].y)/det_T, (vertices[2].x - vertices[1].x)/det_T},
		{(vertices[2].y - vertices[0].y)/det_T, (vertices[0].x - vertices[2].x)/det_T},
	};

	ret[0] = T_inv[0][0] * (P.x - vertices[2].x) + T_inv[0][1] * (P.y - vertices[2].y);
	ret[1] = T_inv[1][0] * (P.x - vertices[2].x) + T_inv[1][1] * (P.y - vertices[2].y);
	ret[2] = 1 - ret[0] - ret[1];
	return ret;
}

vec3 get_barycentric(std::array<vec4, 3> vertices, vec2i P){
	// Initializing the vector that will contain
	// barycentric coordinates
	vec3 ret = {};

	double det_T = (vertices[1].y - vertices[2].y)*(vertices[0].x - vertices[2].x) + (vertices[2].x - vertices[1].x)*(vertices[0].y - vertices[2].y);
	double T_inv[2][2] = {
		{(vertices[1].y - vertices[2].y)/det_T, (vertices[2].x - vertices[1].x)/det_T},
		{(vertices[2].y - vertices[0].y)/det_T, (vertices[0].x - vertices[2].x)/det_T},
	};

	ret[0] = T_inv[0][0] * (P.x - vertices[2].x) + T_inv[0][1] * (P.y - vertices[2].y);
	ret[1] = T_inv[1][0] * (P.x - vertices[2].x) + T_inv[1][1] * (P.y - vertices[2].y);
	ret[2] = 1 - ret[0] - ret[1];
	return ret;
}
