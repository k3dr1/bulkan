#pragma once
#include <cstdint>
#include <memory>

#include "tgaimage.cpp"

template<class pixel_T>
class Image {
public:
	// Constructor
	Image(unsigned int _width, unsigned int _height)
		: width(_width),
		height(_height),
		data(
				std::make_unique<pixel_T[]>(width*height)
		    )
	{}
	// Constructor from TGAImage
	Image(TGAImage tga_image)
		: width(tga_image.width()),
		height(tga_image.height()),
		data(
				std::make_unique<pixel_T[]>(width*height)
		    )
	{
		if (tga_image.get_bpp() == 3){
			std::cout << "Creating Image from tga_image with bpp=3" << '\n';
			for (int i = 0; i < tga_image.width()*tga_image.height(); i++){
				// TGA uses bgr
				// refer to the color guide in the main file
				*((std::uint8_t*)data.get() + 4*i + 3) = 0xff;
				memcpy(((std::uint8_t*)data.get()) + 4*i + 0, ((std::uint8_t*)tga_image.get_data()) + 3*i + 2, sizeof(std::uint8_t));
				memcpy(((std::uint8_t*)data.get()) + 4*i + 1, ((std::uint8_t*)tga_image.get_data()) + 3*i + 1, sizeof(std::uint8_t));
				memcpy(((std::uint8_t*)data.get()) + 4*i + 2, ((std::uint8_t*)tga_image.get_data()) + 3*i + 0, sizeof(std::uint8_t));
			}
		} else if (tga_image.get_bpp() == 4){
			std::cout << "Creating Image from tga_image with bpp=4" << '\n';
			for (int i = 0; i < tga_image.width()*tga_image.height(); i++){
				// TGA uses bgra
				// refer to the color guide in the main file
				memcpy(((std::uint8_t*)data.get()) + 4*i + 3, ((std::uint8_t*)tga_image.get_data()) + 4*i + 3, sizeof(std::uint8_t));
				memcpy(((std::uint8_t*)data.get()) + 4*i + 0, ((std::uint8_t*)tga_image.get_data()) + 4*i + 2, sizeof(std::uint8_t));
				memcpy(((std::uint8_t*)data.get()) + 4*i + 1, ((std::uint8_t*)tga_image.get_data()) + 4*i + 1, sizeof(std::uint8_t));
				memcpy(((std::uint8_t*)data.get()) + 4*i + 2, ((std::uint8_t*)tga_image.get_data()) + 4*i + 0, sizeof(std::uint8_t));
			}
		} else if (tga_image.get_bpp() == 1) {
			std::cout << "Creating Image from tga_image with bpp=1" << '\n';
			for (int i = 0; i < tga_image.width()*tga_image.height(); i++){
				// TGA uses 1 byte grayscale here (0-255)
				// I will just store that in the red section of std::uint32_t
				memcpy((std::uint8_t*)data.get() + 4*i, ((std::uint8_t*)tga_image.get_data()) + i, sizeof(std::uint8_t));
			}
		} else {
			std::cerr << "Weird bpp encountered while initializing Image<std::uint32_t>, bpp = " << tga_image.get_bpp() << '\n';
		}

	}

	// Member functions
	pixel_T& operator[](const unsigned int idx) {
		if (idx > height*width){
			std::cerr << "Accessing out of bounds with operator[]\n";
		}
		return data[idx];
	}
	pixel_T get_pixel(const unsigned int x, const unsigned int y) const {
		if (x < width && y < height){
			return data[y*width + x];
		}
		std::cerr << "Accessing out of bounds in Image<pixel_T>::get_pixel" << '\n';
		return pixel_T();
	}
	void set_pixel(const unsigned int x, const unsigned int y, const pixel_T color) const {
		if (x < width && y < height){
			data[y*width + x] = color;
			return;
		}
		std::cerr << "Accessing out of bounds in Image<pixel_T>::set_pixel" << '\n';
		return;
	}
	int nbytes() const {
		return (width * height * sizeof(pixel_T));
	};

	const unsigned int width;
	const unsigned int height;
	std::unique_ptr<pixel_T[]> const data;
};

template<>
Image<std::uint8_t>::Image(TGAImage tga_image)
	: width(tga_image.width()),
	height(tga_image.height()),
	data(
		std::make_unique<std::uint8_t[]>(width*height)
)
{
	if (tga_image.get_bpp() == 1){
		std::cout << "Creating Image from tga_image with bpp=1" << '\n';
		for (int i = 0; i < tga_image.width()*tga_image.height(); i++){
			data.get()[i] = *(tga_image.get_data() + i);
			//memcpy((std::uint8_t*)data.get(), tga_image.get_data() + i, sizeof(std::uint8_t));
		}
	} else {
		std::cerr << "Weird bpp encountered while initializing Image<std::uint8_t>, bpp = " << tga_image.get_bpp() << '\n';
	}
}
