#include "Utility.h"

namespace Elysium
{
    namespace Utility
    {
        void CreateRandomVector(std::vector<float>& vector, size_t size, float min, float max)
        {
            vector.reserve(size);
            for (size_t i = 0; i < size; ++i)
                vector.emplace_back(Elysium::Random::Float() * (max - min) + min);
        }

        void RandomizeVector(std::vector<float>& vector, float min, float max)
        {
            for (size_t i = 0; i < vector.size(); ++i)
                vector[i] = Elysium::Random::Float() * (max - min) + min;
        }
    }
}