#include <vector>

const auto CoolGradientPosterization = std::vector<std::pair<std::uint32_t, float>>{
	{0xFF000000, 0.15},
	{0xFF403020, 0.30},
	{0xFF804000, 0.50},
	{0xFFB07020, 0.70},
	{0xFFF0A050, 0.85},
	{0xFFFFD090, 1.00}
};
const auto WarmGradientPosterization = std::vector<std::pair<std::uint32_t, float>>{
	{0xFF000000, 0.15},
	{0xFF203040, 0.30},
	{0xFF406080, 0.50},
	{0xFF6090B0, 0.70},
	{0xFF80B0E0, 0.85},
	{0xFFA0D0FF, 1.0}
};
const auto GrayTonesPosterization = std::vector<std::pair<std::uint32_t, float>>{
	{0xFF101010, 0.25},
	{0xFF202020, 0.50},
	{0xFF303030, 0.75},
	{0xFF404040, 1.00},
};
const auto DarkBlueToOrangePosterization = std::vector<std::pair<std::uint32_t, float>>{
	{0xFF000000, 0.20},
	{0xFF2C190B, 0.45},
	{0xFF623E1E, 0.70},
	{0xFF0065FF, 1.00},
};
const auto RandomPosterization = std::vector<std::pair<std::uint32_t, float>>{
	{0xFF6C7059, 0.04},
	{0xFF1E213D, 0.07},
	{0xFFFAD201, 0.10},
	{0xFF3D642D, 0.20},
	{0xFFF39F18, 0.40},
	{0xFF193737, 0.80},
	{0xFFFFFFFF, 1.00},
};
const auto LowBandPassPosterization = std::vector<std::pair<std::uint32_t, float>>{
	{0xFF000000, 0.10},
	{0xFF39FF14, 0.25},
	{0xFF000000, 1.00},
};
