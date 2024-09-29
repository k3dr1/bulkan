#pragma once
#include <fstream>
#include <iostream>
#include <string>

#include "./model.h"

namespace ObjParser {
	constexpr bool is_num(const char c);

	void skip_ws(const std::string& s, unsigned int& idx);
	void skip_to_ws(const std::string& s, unsigned int& idx);
	void skip_to_fs(const std::string& s, unsigned int& idx);
	void skip_num(const std::string& s, unsigned int& idx);
	
	void v(std::string& line, Model* mdl);
	void vt(std::string& line, Model* mdl);
	void vn(std::string& line, Model* mdl);
	void f(std::string& line, Model* mdl);
}

int parse_obj(std::string filepath, Model* mdl);
