#include "Random.h"

std::mt19937 Elysium::Random::s_RandomEngine;
std::uniform_int_distribution<std::mt19937::result_type> Elysium::Random::s_Distribution;
std::uniform_int_distribution<int> Elysium::Random::s_IntegerDistribution;