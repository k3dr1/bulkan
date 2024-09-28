#pragma once
#include <array>
#include <cmath>
#include <cassert>
#include <cstdint>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>

#include "image.h"
#include "mat_vec.h"
#include "model.h"

// Shaderclass which consists of an overwritable destructor
// and pure virtual vertex/fragment functions representing corresponding shaders
template<class pixel_T>
struct ShaderClass {
	virtual ~ShaderClass(){};
	virtual vec4 vertex(int iface, int nthvert) = 0;
	virtual bool fragment(vec3 bary, pixel_T& pixel) = 0;
};

// Applies the fn function to first three bytes (red, green, and blue)
// be wary of the rollover
std::uint32_t modify_channels(std::uint32_t color, std::uint8_t (*fn)(std::uint8_t));

// Multiply each channel separately by scale
// limits it from above to 255 in each channel
std::uint32_t color_mult(std::uint32_t color, double scale);

int   clamp(int low, int val, int high);
float clampf(float low, float val, float high);

// Reflects the vector "v" across the vector "line"
template<unsigned int n>
vec<n> reflect(vec<n> v, vec<n> line){
	return vec<n>(2 * ((v*line)/(line*line)) * line - v);
}

mat<4,4> look_at(vec<3> eye, vec<3> center, vec<3> up);

// Returns the matrix which bounds (-1,+1)*(-1,+1)*(-1,+1) to (x, y)*(x+w, y+h)*(0,d)
// -1 in the entry (1,1) needed to flip vertically
mat<4,4> get_viewport(int x, int y, int w, int h, int d);

mat<4,4> get_projection(float c);

template<class pixel_T>
int draw_line(pixel_T color, Image<pixel_T>& canvas, int x0, int y0, int x1, int y1){
	// Bounds checking
	if ((x0 < 0) || (x1 < 0) || (y0 < 0) || (y1 < 0) ||
			(x0 >= canvas.width) || (x1 >= canvas.width) ||
			(y0 >= canvas.height) || (y1 >= canvas.height)) {
		return -1;
	}
	bool steep = false;
	if (std::abs(x0-x1) < std::abs(y0-y1)){
		std::swap(x0, y0);
		std::swap(x1, y1);
		steep = true;
	}
	if (x1 < x0){
		std::swap(x0, x1);
		std::swap(y0, y1);
	}
	// dx is always positive because we made sure that x1 > x0
	int dx = x1 - x0;
	int dy = y1 - y0;
	int derror = std::abs(2*dy);
	int error = 0;
	int y = y0;
	for (int x = x0; x < x1; x++){
		if (steep){
			canvas[canvas.width*x + y] = color;
		} else {
			canvas[canvas.width*y + x] = color;
		}
		error += derror;
		if (error > dx){
			y += (y1 > y0 ? 1 : -1);
			error -= 2*dx;
		}
	}
	return 0;
}

vec3 get_barycentric(std::array<vec3, 3> vertices, vec2i P);

vec3 get_barycentric(std::array<vec4, 3> vertices, vec2i P);

template<class pixel_T>
void draw_triangle_textured_illuminated(std::array<vec4, 3> triangle_positions, std::array<vec3, 3> triangle_textures, std::array<vec3, 3> triangle_normals, Image<double>& zbuffer, Image<pixel_T>& canvas, Image<pixel_T>& texture, vec3 lighting_vector){
	// bbox[0] is the inner point
	// bbox[1] is the outer point
	vec2i bbox[2] = {{.x = (int)canvas.width, .y = (int)canvas.height}, {.x = 0, .y = 0}}; // These are just initial values, they will be changed below
	for (int i = 0; i < 3; i++){
		bbox[0].x = triangle_positions[i][0] < bbox[0].x ? triangle_positions[i][0] : bbox[0].x;
		bbox[0].y = triangle_positions[i][1] < bbox[0].y ? triangle_positions[i][1] : bbox[0].y;

		bbox[1].x = triangle_positions[i][0] > bbox[1].x ? triangle_positions[i][0] : bbox[1].x;
		bbox[1].y = triangle_positions[i][1] > bbox[1].y ? triangle_positions[i][1] : bbox[1].y;
	}

	// Barycentric coordinates
	vec3 barycords;
	// Point inside the bounding box
	vec2i P = {.x = bbox[0].x, .y = bbox[0].y};
	double zdepth = 0;
	// Color
	pixel_T color;
	pixel_T illuminated_color;
	vec3 normal;
	double lighting_intensity;

	// Looping over every single pixel in the box
	for (; P.x <= bbox[1].x; P.x++){
		P.y = bbox[0].y;
		for (; P.y <= bbox[1].y; P.y++){
			barycords = get_barycentric(triangle_positions, P);
			if ((barycords.x < 0) || (barycords.y < 0) || (barycords.z < 0)) continue;

			// Experimental
			if (P.x < 0 || P.y < 0 || (int)canvas.width <= P.x || (int)canvas.height <= P.y) continue;

			normal = barycords[0]*triangle_normals[0] + barycords[1]*triangle_normals[1] + barycords[2]*triangle_normals[2];
			lighting_intensity = normal*lighting_vector;

			if (lighting_intensity <= 0) continue;

			zdepth = barycords.x * triangle_positions[0][2] + barycords.y * triangle_positions[1][2] + barycords.z * triangle_positions[2][2];
			if (zdepth > zbuffer[P.y*canvas.width + P.x]){


				int texture_x = barycords[0]*triangle_textures[0][0] + barycords[1]*triangle_textures[1][0] + barycords[2]*triangle_textures[2][0];
				int texture_y = barycords[0]*triangle_textures[0][1] + barycords[1]*triangle_textures[1][1] + barycords[2]*triangle_textures[2][1];
				color = texture[texture_y*texture.width + texture_x];
				illuminated_color = 0xff'00'00'00;

				illuminated_color += ((pixel_T)(lighting_intensity*((color & 0x000000ff)>>(8*0))))*(1);
				illuminated_color += ((pixel_T)(lighting_intensity*((color & 0x0000ff00)>>(8*1))))*(256);
				illuminated_color += ((pixel_T)(lighting_intensity*((color & 0x00ff0000)>>(8*2))))*(256*256);

				canvas[P.y*canvas.width + P.x] = illuminated_color;
				zbuffer[P.y*canvas.width + P.x] = zdepth;
			}
		}
	}
}

template<class pixel_T>
void draw_shaded_triangle(std::array<vec4, 3> screen_coords, ShaderClass<pixel_T>& shader, Image<pixel_T>& canvas, Image<double>& zbuffer){
	// bbox[0] is the inner point
	// bbox[1] is the outer point
	vec2i bbox[2] = {{.x = (int)canvas.width, .y = (int)canvas.height}, {.x = 0, .y = 0}}; // These are initial values, they will be changed below
	for (int i = 0; i < 3; i++){
		bbox[0].x = screen_coords[i][0] < bbox[0].x ? screen_coords[i][0] : bbox[0].x;
		bbox[0].y = screen_coords[i][1] < bbox[0].y ? screen_coords[i][1] : bbox[0].y;

		bbox[1].x = screen_coords[i][0] > bbox[1].x ? screen_coords[i][0] : bbox[1].x;
		bbox[1].y = screen_coords[i][1] > bbox[1].y ? screen_coords[i][1] : bbox[1].y;
	}

	vec2i P = {.x = bbox[0].x, .y = bbox[0].y};
	vec3 barycords;
	double zdepth = 0;
	pixel_T color;
	bool discard;

	// Looping over every single pixel in the box
	for (; P.x <= bbox[1].x; P.x++){
		P.y = bbox[0].y;
		for (; P.y <= bbox[1].y; P.y++){
			barycords = get_barycentric(screen_coords, P);
			if ((barycords.x < 0) || (barycords.y < 0) || (barycords.z < 0)) continue; // Outside the triangle
			if (P.x < 0 || P.y < 0 || (int)canvas.width <= P.x || (int)canvas.height <= P.y) continue; // Outside the screen


			zdepth = barycords.x * screen_coords[0][2] + barycords.y * screen_coords[1][2] + barycords.z * screen_coords[2][2];
			if (zdepth > zbuffer[P.y*canvas.width + P.x]){
				discard = shader.fragment(barycords, color);
				if (!discard){
					canvas[P.y*canvas.width + P.x] = color;
					zbuffer[P.y*canvas.width + P.x] = zdepth;
				}
			}
		}
	}
}
template<class pixel_T>
bool draw_model(Model& mdl, mat<4,4>& modelview, mat<4,4>& projection, mat<4,4>& viewport, vec3 light_dir, Image<pixel_T>& canvas, Image<double>& zbuffer, Image<pixel_T>& texture){
	std::array<vec<4>, 3> screen_coords;
	std::array<vec<3>, 3> ss_texture;
	std::array<vec<3>, 3> normals;
	for (size_t face = 0; face < mdl.face_vrtx.size(); face += 3){
		// Reading a face (triangle)
		for (int i = 0; i < 3; i++){
			// Vertex shader is executed here 
			screen_coords.at(i)[0] = mdl.verts[mdl.face_vrtx[face + i]][0];
			screen_coords.at(i)[1] = mdl.verts[mdl.face_vrtx[face + i]][1];
			screen_coords.at(i)[2] = mdl.verts[mdl.face_vrtx[face + i]][2];
			screen_coords.at(i)[3] = 1;

			screen_coords.at(i) = viewport*projection*modelview*screen_coords.at(i);

			screen_coords.at(i)[0] /= screen_coords.at(i)[3];
			screen_coords.at(i)[1] /= screen_coords.at(i)[3];
			screen_coords.at(i)[2] /= screen_coords.at(i)[3];
			screen_coords.at(i)[3] /= screen_coords.at(i)[3];

			// Similar transformation with texture coords
			ss_texture.at(i) = mdl.tex_coords[mdl.face_tex[face + i]];
			ss_texture.at(i) = mdl.tex_coords[mdl.face_tex[face + i]];

			ss_texture.at(i).x = (ss_texture.at(i).x * texture.width);
			ss_texture.at(i).y = texture.height - (ss_texture.at(i).y * texture.height);

			ss_texture.at(i).x = std::min(ss_texture.at(i).x, double(texture.width - 1));
			ss_texture.at(i).y = std::min(ss_texture.at(i).y, double(texture.height - 1));

			// Getting normals of the face
			normals[i] = mdl.normals[mdl.face_norm[face + i]];
		}
		draw_triangle_textured_illuminated(screen_coords, ss_texture, normals, zbuffer, canvas, texture, light_dir);
	}
	return true;
}

template<class pixel_T>
void img_fill(Image<pixel_T>& canvas, pixel_T color){
	for (unsigned int pixel_idx = 0; pixel_idx < canvas.width*canvas.height; pixel_idx++){
		canvas[pixel_idx] = color;
	}
}

template<class pixel_T>
int img_save(std::string filepath, Image<pixel_T>& canvas){
	std::uint8_t bytes[3] {0,0,0};
	std::ofstream file;
	file.open(filepath, std::ios::out | std::ios::binary);
	if (file.is_open()){
		file << "P6\n" << canvas.width << " " << canvas.height << " " << "255" << "\n";
		for (unsigned int i = 0; i < canvas.width*canvas.height; i++){
			bytes[0] = (canvas[i]>>(8*0))&0xff;
			bytes[1] = (canvas[i]>>(8*1))&0xff;
			bytes[2] = (canvas[i]>>(8*2))&0xff;
			file.write((char*)bytes, 3*(sizeof(*bytes)));
		}
		return 0;
	} else {
		std::cerr << "Error with file in img_save\n";
		return -1;
	}
}

