#include <fstream>
#include <iostream>
#include <string>
#include "./model.h"
#include "./parser.h"

// Helper Functions

namespace ObjParser {

constexpr bool is_num(const char c) {
	return (47 < c) && (c < 58);
}

// Starts at a whitespace and continues until a non-whitespace character is found
void skip_ws(const std::string& s, unsigned int& idx){
	if (s[idx] != ' ') return;
	for (; idx < s.size(); idx++){
		if (s[idx] != ' ') return;
	}
	idx++;
	return;
}

// Starts at a non-whitespace and stops on a whitespace
void skip_to_ws(const std::string& s, unsigned int& idx){
	for (; idx < s.size(); idx++){
		if (s[idx] == ' ') return;
	}
	idx++;
	return;
}

// Starts at a character that is not a forward-slash and stops on a forward-slash
// if it starts on a forward-slash, then it stops on the next forward slash
// if no forward-slashes are found after the given index, it returns npos
void skip_to_fs(const std::string& s, unsigned int& idx){
	for (; idx < s.size(); idx++){
		if (s[idx] == '/') return;
	}
	idx++;
	return;
}

// Leaves the idx on the first non-numeric value
// (numeric: 0..9)
void skip_num(const std::string& s, unsigned int& idx){
	for (; idx < s.size(); idx++){
		if (s[idx] <= 47 || 58 <= s[idx]) return;
	}
	idx++;
	return;
}

void v(std::string& line, Model* mdl){
	// Skips the "v" part
	unsigned int idx = 1;
	vec3 vertex_position;
	skip_ws(line, idx);
	vertex_position.x = std::stof(line.substr(idx));

	skip_to_ws(line, idx);
	skip_ws(line, idx);
	vertex_position.y = std::stof(line.substr(idx));

	skip_to_ws(line, idx);
	skip_ws(line, idx);
	vertex_position.z = std::stof(line.substr(idx));

	mdl->verts.push_back(vertex_position);
}

void vt(std::string& line, Model* mdl){
	// Skips the "vt" part
	unsigned int idx = 2;
	vec3 vertex_uv;
	skip_ws(line, idx);
	vertex_uv.x = std::stof(line.substr(idx));

	skip_ws(line, idx);
	skip_to_ws(line, idx);
	vertex_uv.y = std::stof(line.substr(idx));

	mdl->tex_coords.push_back(vertex_uv);
}

void vn(std::string& line, Model* mdl){
	// Skips the "vn" part
	unsigned int idx = 2;
	vec3 vertex_normal;
	skip_ws(line, idx);
	vertex_normal.x = std::stof(line.substr(idx));

	skip_to_ws(line, idx);
	skip_ws(line, idx);
	vertex_normal.y = std::stof(line.substr(idx));

	skip_to_ws(line, idx);
	skip_ws(line, idx);
	vertex_normal.z = std::stof(line.substr(idx));

	// Making the length of the vector equal to 1
	mdl->normals.push_back(vertex_normal/vertex_normal.norm());
}

void f(std::string& line, Model* mdl){
	// Skips the "f" part
	unsigned int idx = 1;

	skip_ws(line, idx);
	mdl->face_vrtx.push_back(
		std::stoi(line.substr(idx)) - 1);
	skip_num(line, idx);
	if (line[idx] == '/') {
		if (is_num(line[idx+1])) {
			idx++;
			mdl->face_tex.push_back(
				std::stoi(line.substr(idx)) - 1);
			skip_num(line, idx);
		} else {
			idx++;
		}
		if (line[idx] == '/') {
			idx++;
		if (is_num(line[idx])) {
				mdl->face_norm.push_back(
				std::stoi(line.substr(idx)) - 1);
				skip_num(line, idx);
			}
		}
	}

	skip_ws(line, idx);
	mdl->face_vrtx.push_back(
		std::stoi(line.substr(idx)) - 1);
	skip_num(line, idx);
	if (line[idx] == '/') {
		if (is_num(line[idx+1])) {
			idx++;
			mdl->face_tex.push_back(
				std::stoi(line.substr(idx)) - 1);
			skip_num(line, idx);
		} else {
			idx++;
		}
		if (line[idx] == '/') {
			idx++;
		if (is_num(line[idx])) {
				mdl->face_norm.push_back(
				std::stoi(line.substr(idx)) - 1);
				skip_num(line, idx);
			}
		}
	}

	skip_ws(line, idx);
	mdl->face_vrtx.push_back(
		std::stoi(line.substr(idx)) - 1);
	skip_num(line, idx);
	if (line[idx] == '/') {
		if (idx + 1 < line.size() && is_num(line[idx+1])) {
			idx++;
			mdl->face_tex.push_back(
				std::stoi(line.substr(idx)) - 1);
			skip_num(line, idx);
		} else if (idx + 1 < line.size()) {
			idx++;
		} else {}
		if (line[idx] == '/') {
			idx++;
		if (is_num(line[idx])) {
				mdl->face_norm.push_back(
				std::stoi(line.substr(idx)) - 1);
				skip_num(line, idx);
			}
		}
	}
}

} // namespace ObjParser

int parse_obj(std::string filepath, Model* mdl){
	std::ifstream file;
	std::string line;
	std::string line_state;
	file.open(filepath, std::ios::in);

	if(!file.is_open()){
		std::cerr << "PARSER: Problem parsing the .obj file: could not open" << '\n';
		return -1;
	} 

	while (!file.eof()) {
		std::getline(file, line, '\n');
		line_state = line.substr(0, 2);
		if (line_state == "v "){
			ObjParser::v(line, mdl);
			continue;
		}
		if (line_state == "vt"){
			ObjParser::vt(line, mdl);
			continue;
		}
		if (line_state == "vn"){
			ObjParser::vn(line, mdl);
			continue;
		}
		if (line_state == "f "){
			ObjParser::f(line, mdl);
			continue;
		}
		std::cout << "PARSER: Weird line encountered [" << line << "]\n";
	}
	return 0;
}
