#include <cstdint>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>

#include "image.cpp"

int main(){

	std::uint32_t best_color = 0xFF10AF10;
	std::cout << "the best color is " << best_color << '\n';

	{
		Image<std::uint32_t> my_image(400, 200);

		for (int x = 0; x < my_image.width; x++){
			for (int y = 0; y < my_image.height; y++){
				my_image.set_pixel(x, y, best_color);
			}
		}

		std::cout << "Type of the data " << typeid(my_image.data).name();

		//std::cout << "The number of bytes is " << my_image.nbytes() << '\n';
	}

	return 0;
}
