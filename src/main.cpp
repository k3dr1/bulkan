#include <algorithm>
#include <array>
#include <limits>
#include <random>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <cmath>

#include "./renderer.h"
#include "./model.h"
#include "./tgaimage.h"
#include "./parser.h"
#include "./image.h"
#include "./mat_vec.h"
#include "./mat_vec.cpp"

// Color guide:
// 0xAABBGGRR in hex notation within a uint32
// (RED)(GREEN)(BLUE)(ALPHA) in byte order

#define WIDTH  (1000)
#define HEIGHT (1000)
#define FOREGROUND_COLOR 0xFFFFFFFF
#define BACKGROUND_COLOR 0xFF000000

// File scope declarations
Model mdl;
vec3 light_dir;
mat<4,4> Viewport;
mat<4,4> Projection;
mat<4,4> ModelView;

struct DepthShader : public ShaderClass<std::uint32_t> {
	mat<3,3> varying_tri;

	DepthShader() : varying_tri() {}

	vec<4> vertex(int iface, int nthvert) override {
		vec<4> gl_Vertex = embed<4>(mdl.verts[mdl.face_vrtx[iface + nthvert]]);
		gl_Vertex = Viewport*Projection*ModelView*gl_Vertex;
		
	}
};

struct TextureTangentNormalShader : public ShaderClass<std::uint32_t>{
	int uniform_ambient;
	mat<3,3> varying_nrm;
	mat<3,2> varying_uv;
	/*
	[u0, v0],
	[u1, v1],
	[u2, v2],
	*/
	mat<4,4> uniform_M; // Projection*ModelView
	mat<4,4> uniform_M_IT; // Projection*ModelView invert_transpose()
	mat<3,3> ndc_tri; // Vertices in normalized device coords, each vector is separate row

	vec<4> vertex(int iface, int nthvert) override {
		varying_nrm[nthvert] = mdl.normals[mdl.face_norm[iface + nthvert]];
		varying_uv[nthvert] = proj<2>(mdl.tex_coords[mdl.face_tex[iface + nthvert]]);

		vec4 gl_Vertex = embed<4>(mdl.verts[mdl.face_vrtx[iface + nthvert]]);
		ndc_tri[nthvert] = proj<3>((Projection*ModelView*gl_Vertex).w_normalized());

		return (Viewport*Projection*ModelView*gl_Vertex).w_normalized();
	}

	bool fragment(vec3 barycentric, std::uint32_t& color) override {
		vec2 uv = (varying_uv.transpose()) * barycentric;
		vec3 surface_normal = (varying_nrm.transpose() * barycentric).normalized();
		
		mat<3,3> A;
		A[0] = ndc_tri[1] - ndc_tri[0];
		A[1] = ndc_tri[2] - ndc_tri[0];
		A[2] = surface_normal;

		mat<3,3> AI = A.invert();

		vec3 i = AI * vec3(varying_uv[1][0] - varying_uv[0][0], varying_uv[2][0] - varying_uv[0][0], 0);
		vec3 j = AI * vec3(varying_uv[1][1] - varying_uv[0][1], varying_uv[2][1] - varying_uv[0][1], 0);

		// Matrix for change of basis from tangent to object coords
		mat<3,3> B;
		B.set_col(0, i.normalized());
		B.set_col(1, j.normalized());
		B.set_col(2, surface_normal);

		// Transforming tangent-space normals to object coords
		vec3 normal = (B*mdl.get_normal(uv)).normalized();

		vec3 n = proj<3>(uniform_M_IT*embed<4>(normal)).normalized(); // transformed normal
		vec3 l = proj<3>(uniform_M   *embed<4>(light_dir)).normalized(); // transformed light_dir
		vec3 r = (n*(2.f*n*l) - l).normalized(); // l reflected across the n
		std::uint32_t temp_color = mdl.get_texture(uv);

		float diffuse = std::max(0.0, n*l);
		// we take the z component because the camera is on the z-axis after the transformation
		float specular = std::max(0.0, std::pow(r.z, mdl.get_specular(uv)));
		std::uint8_t* temp_p = (std::uint8_t*)&temp_color;
		std::uint8_t* color_p = (std::uint8_t*)&color;
		for (int i = 0; i < 3; i++){
			color_p[i] = uniform_ambient + temp_p[i]*(1.0*diffuse + 0.6*specular);
		}
		return false;

	};
};

int main(){

	int parse_status;
	parse_status = parse_obj("./res/african_head.obj", &mdl);
	if (parse_status == -1){
		std::cerr << "Error in the parse\n";
		return -1;
	}

	Image<std::uint32_t> pixels(WIDTH, HEIGHT);
	Image<double> zbuffer(WIDTH, HEIGHT);
	img_fill(pixels , BACKGROUND_COLOR);
	img_fill(zbuffer, std::numeric_limits<double>::lowest());

	TGAImage man_texture;
	man_texture.read_tga_file("./res/african_head_diffuse.tga");
	man_texture.write_tga_file("tga_texture_man.tga");
	Image<std::uint32_t> texture(man_texture);

	TGAImage man_normals;
	man_normals.read_tga_file("./res/african_head_nm.tga");
	man_normals.write_tga_file("tga_normals_man.tga");
	Image<std::uint32_t> normals(man_normals);

	TGAImage man_tangent_normals;
	man_tangent_normals.read_tga_file("./res/african_head_nm_tangent.tga");
	man_tangent_normals.write_tga_file("tga_tangent_normals_man.tga");
	Image<std::uint32_t> tangent_normals(man_tangent_normals);

	TGAImage man_specular;
	man_specular.read_tga_file("./res/african_head_spec.tga");
	man_specular.write_tga_file("tga_specular_man.tga");
	Image<std::uint32_t> specular(man_specular);

	vec3 eye    = vec3{0.3, 0.3, 1.0};
	vec3 center = vec3{0, 0, 0};
	vec3 up     = vec3{0, 1, 0};
	double c = 3;

	light_dir  = {0.8, 0.0, 1.0};
	ModelView  = look_at(eye, center, up);
	Projection = get_projection(c);
	Viewport   = get_viewport(0, 0, WIDTH, HEIGHT, 255); 

	std::cout << "Generated modelview matrix: \n" << ModelView << '\n';
	std::cout << "Generated projection matrix: \n" << Projection << '\n';
	std::cout << "Generated viewport matrix: \n" << Viewport << '\n';

	//GouraudShader shader;
	//TextureShader shader(texture);
	//TextureNormalShader shader;
	TextureTangentNormalShader shader;
	shader.uniform_M = Projection*ModelView;
	shader.uniform_M_IT = (Projection*ModelView).invert_transpose();
	shader.uniform_ambient = 5;

	mdl.m_texturemap = &texture;
	//mdl.m_normalmap = &normals;
	mdl.m_normalmap = &tangent_normals;
	mdl.m_specularmap = &specular;

	std::array<vec<4>, 3> screen_coords;
	for (size_t iface = 0; iface < mdl.face_vrtx.size(); iface += 3){
		// Reading a face (triangle)
		for (int nthvert = 0; nthvert < 3; nthvert++){
			screen_coords[nthvert] = shader.vertex(iface, nthvert);
		}
		draw_shaded_triangle(screen_coords, shader, pixels, zbuffer);
	}

	std::string image_name = "output";
	image_name.append(".ppm");
	img_save(image_name, pixels);
	std::cout << "Completed the render!\n";

	//img_save("tangent_normals.ppm", tangent_normals);
	//img_save("normals.ppm", normals);
	//img_save("texture.ppm",  texture);
	//img_save("specular.ppm",  specular);

	return 0;
}
