#pragma once

#include <random>

namespace Elysium
{
	class Random
	{
	private:
		static std::mt19937 s_RandomEngine;
		static std::uniform_int_distribution<std::mt19937::result_type> s_Distribution;
		static std::uniform_int_distribution<int> s_IntegerDistribution;

	public:
		static void Init()
		{
			s_RandomEngine.seed(std::random_device()());
		}

		static float Float()
		{
			return (float)s_Distribution(s_RandomEngine) / (float)std::numeric_limits<uint32_t>::max();
		}

		static void InitInteger(int min, int max)
		{
			s_IntegerDistribution = std::uniform_int_distribution<int>(min, max);
		}

		static int Integer()
		{
			return s_IntegerDistribution(s_RandomEngine);
		}
	};
}