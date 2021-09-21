#pragma once

#include <deque>

#include "BinaryFeatureVector.h"
#include "RLAgent.h"

namespace Elysium
{
    template <size_t N>
    class LinearRLAgent : public RLAgent
    {
    protected:
        std::array<float, N> WeightVector;

    public:
        LinearRLAgent(float learningRate, float discountFactor, float defaultValue) :
            RLAgent(learningRate, discountFactor, defaultValue)

        {
            WeightVector.fill(defaultValue);
        }

        float getValue(const BinaryFeatureVector<N>& currentFeatureVector)
        {
            float currentStateValue = 0.0f;
            #pragma omp simd
            for (size_t i = 0; i < N; i++)
            {
                currentStateValue += WeightVector[i] * currentFeatureVector[i];
            }
            return currentStateValue;
        }

        void updateWeightVectorSarsa(const BinaryFeatureVector<N>& currentFeatureVector, const BinaryFeatureVector<N>& nextFeaturesVector,
            const State& nextState)
        {
            float currentValue = getValue(currentFeatureVector);

            float nextValue = 0.0f;
            if (!nextState.isTerminal())
                nextValue = getValue(nextFeaturesVector);

            #pragma omp simd
            for (size_t i = 0; i < N; i++)
            {
                WeightVector[i] += LearningRate * (nextState.getReward() + (DiscountFactor * nextValue) - currentValue) * (float)currentFeatureVector[i];
            }
        }

        bool readWeightVectorFromFile(const char* filename)
        {
            bool isOpen = false;
            std::ifstream file(filename);
            if (file.is_open())
            {
                isOpen = true;
                std::string line;
                for (size_t i = 0; i < N; i++)
                {
                    if (getline(file, line))
                        WeightVector[i] = std::stof(line);
                }
                file.close();
            }
            return isOpen;
        }

        void saveWeightVectorToFile(const char* filename)
        {
            std::ofstream file(filename);
            std::string line;
            for (size_t i = 0; i < N; i++)
            {
                file << WeightVector[i] << std::endl;;
            }
            file.close();
        }
    };
}