#pragma once
#include <cstdint>
#include <vector>
#include "image.h"
#include "mat_vec.h"
#include "tgaimage.h"

class Model{
public:
	int nverts() const {
		return this->verts.size();
	}
	int nfaces() const{
		return this->face_vrtx.size()/3;
	}
	std::vector<vec3> verts{};
	std::vector<vec3> tex_coords{};
	std::vector<vec3> normals{};
	// These contain the indices of vertices
	// each face has 3 vertices, they are not separated in the vector
	std::vector<int> face_vrtx; 
	std::vector<int> face_tex; 
	std::vector<int> face_norm; 

	Image<std::uint32_t>* m_texturemap;
	Image<std::uint32_t>* m_normalmap;
	Image<std::uint32_t>* m_specularmap;

	// Accesses the texture map
	// uv.y is inverted (1-uv.y)
	std::uint32_t get_texture(vec2 uv) const {
		return m_texturemap->get_pixel(std::min(m_texturemap->width*uv.x, (double)m_texturemap->width-1), 
				  std::min(m_texturemap->height*(1-uv.y), (double)m_texturemap->height-1));
	}

	// Accesses the normal map
	// uv.y is inverted (1-uv.y)
	vec3 get_normal(vec2 uv) const {
		std::uint32_t color = m_normalmap->get_pixel(std::min(m_normalmap->width*uv.x, (double)m_normalmap->width-1), 
				  std::min(m_normalmap->height*(1-uv.y), (double)m_normalmap->height-1));
		vec3 res;
		// x = red, y = green, z = blue
		res[0] = ((color&0x000000ff)>>0)/255.f * 2.f - 1.f;
		res[1] = ((color&0x0000ff00)>>8)/255.f * 2.f - 1.f;
		res[2] = ((color&0x00ff0000)>>16)/255.f * 2.f - 1.f;
		return res;
	}

	std::uint8_t get_specular(vec2 uv) const {
		std::uint8_t exponent = m_specularmap->get_pixel(std::min(m_specularmap->width*uv.x, (double)m_specularmap->width-1), 
			 std::min(m_specularmap->height*(1-uv.y), (double)m_specularmap->height-1));
		return exponent;
	}
};

