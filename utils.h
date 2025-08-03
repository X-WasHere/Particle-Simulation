#pragma once 

#include <random>

// Generate random number
namespace utils
{
	inline float randFloat(int range)
	{
		range *= 100;
		std::random_device rnd;
		std::uniform_int_distribution<int> gen{ -range, range };
		float random_num = static_cast<float>(gen(rnd));

		return random_num / 100;
	}
}
