#pragma once
#include <fstream>
#include <iostream>
#include <string>
#include "./renderer.h"
#include "./model.h"

namespace LineParse {
	void v(std::string& line, Model* mdl){
		int curr;
		int prev;
		vec3 vertex;
		curr = line.find(' ', 2);
		vertex.x = std::stof(line.substr(2, curr - 2));
		prev = curr;

		curr = line.find(' ', prev + 1);
		vertex.y = std::stof(line.substr(prev, curr - prev));
		prev = curr;

		curr = line.find(' ', prev + 1);
		vertex.z = std::stof(line.substr(prev, curr - prev));

		mdl->verts.push_back(vertex);
	}
	void vt(std::string& line, Model* mdl){
		int curr;
		int prev;
		vec3 texture;
		curr = line.find(' ', 4);
		texture.x = std::stof(line.substr(4, curr - 4));
		prev = curr;

		curr = line.find(' ', prev + 1);
		texture.y = std::stof(line.substr(prev, curr - prev));
		prev = curr;

		curr = line.find(' ', prev + 1);
		texture.z = std::stof(line.substr(prev, curr - prev));

		mdl->tex_coords.push_back(texture);
	}
	void vn(std::string& line, Model* mdl){
		int curr;
		int prev;
		vec3 normal;
		curr = line.find(' ', 4);
		normal.x = std::stof(line.substr(4, curr - 4));
		prev = curr;

		curr = line.find(' ', prev + 1);
		normal.y = std::stof(line.substr(prev, curr - prev));
		prev = curr;

		curr = line.find(' ', prev + 1);
		normal.z = std::stof(line.substr(prev, curr - prev));

		mdl->normals.push_back(normal);
	}
	void f(std::string& line, Model* mdl){
		unsigned int grp_beg = 2;
		unsigned int grp_end = 0;
		unsigned int elm_beg;
		unsigned int elm_end;
		for (int i = 0; i < 3; i++){
			grp_end = line.find(' ', grp_beg);

			elm_beg = grp_beg;
			elm_end = line.find('/', elm_beg);
			mdl->face_vrtx.push_back(
					std::stoi(line.substr(elm_beg, elm_end - elm_beg)) - 1
					);
			elm_beg = elm_end + 1;

			elm_end = line.find('/', elm_beg);
			mdl->face_tex.push_back(
					std::stoi(line.substr(elm_beg, elm_end - elm_beg)) - 1
					);
			elm_beg = elm_end + 1;

			mdl->face_norm.push_back(
					std::stoi(line.substr(elm_beg, grp_end - elm_beg)) - 1
					);

			grp_beg = grp_end + 1;
		}
	}
}

int parse_obj(std::string filepath, Model* mdl){
	std::ifstream file;
	std::string line;
	std::string line_state;
	file.open(filepath, std::ios::in);

	if(!file.is_open()){
		return -1;
	} 

	while (!file.eof()){
		std::getline(file, line, '\n');
		line_state = line.substr(0, 2);
		if (line_state == "v "){
			LineParse::v(line, mdl);
			continue;
		}
		if (line_state == "vt"){
			LineParse::vt(line, mdl);
			continue;
		}
		if (line_state == "vn"){
			LineParse::vn(line, mdl);
			continue;
		}
		if (line_state == "f "){
			LineParse::f(line, mdl);
		}
	}
	return 1;
}

// Starts at a whitespace and continues until a non-whitespace character is found
void _skip_ws(const std::string& s, int& idx){
	for (; idx < s.size(); idx++){
		if (s[idx] != ' ') return;
	}
	idx++;
	return;
}

void _skip_to_ws(const std::string& s, int& idx){
	for (; idx < s.size(); idx++){
		if (s[idx] == ' ') return;
	}
	idx++;
	return;
}

int new_parse_obj(std::string filepath, Model* mdl){
	std::ifstream file;
	std::string line;
	file.open(filepath, std::ios::in);

	if(!file.is_open()){
		std::cerr << "Problem parsing the .obj file: could not open" << '\n';
		return -1;
	} 

	while (!file.eof()) {
		std::getline(file, line, '\n');
	}

	return 0;
}
