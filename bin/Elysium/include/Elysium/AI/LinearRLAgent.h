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
        struct Visit
        {
            std::array<float, N> FeatureVector = { 0.0f };
            float Reward = 0.0f;

            Visit(const std::array<float, N>& featureVector, float reward) :
                FeatureVector(featureVector),
                Reward(reward)
            {
            }
        };

        size_t n_Steps = 1;
        std::deque<Visit> Visits;


        std::array<float, N> WeightVector;

    private:
        void updateWeightVectorWhenTerminal()
        {
            float currentValue = 0.0f;
            #pragma omp simd
            for (size_t i = 0; i < N; i++)
            {
                currentValue += WeightVector[i] * Visits.at(0).FeatureVector[i];
            }

            float expectedRewards = 0.0f;
            float discount = 1.0f;
            for (size_t i = 0; i < Visits.size(); i++)
            {
                expectedRewards += discount * Visits.at(i).Reward;
                discount *= DiscountFactor;
            }

            #pragma omp simd
            for (size_t i = 0; i < N; i++)
            {
                WeightVector[i] += LearningRate * (expectedRewards - currentValue) * Visits.at(0).FeatureVector[i];
            }
            Visits.pop_front();
        }

    public:
        LinearRLAgent(float learningRate, float discountFactor, float defaultValue) :
            RLAgent(learningRate, discountFactor, defaultValue)
        {
            WeightVector.fill(defaultValue);
        }

        LinearRLAgent(size_t steps, float learningRate, float discountFactor, float defaultValue) :
            n_Steps(steps),
            RLAgent(learningRate, discountFactor, defaultValue)
        {
            WeightVector.fill(defaultValue);
        }

        float getValue(const std::array<float, N>& currentFeatureVector)
        {
            float currentStateValue = 0.0f;
            #pragma omp simd
            for (size_t i = 0; i < N; i++)
            {
                currentStateValue += WeightVector[i] * currentFeatureVector[i];
            }
            return currentStateValue;
        }

        void updateWeightVectorSarsa(const std::array<float, N>& currentFeatureVector, const std::array<float, N>& nextFeaturesVector,
            const State& nextState)
        {
            float currentValue = 0.0f;
            #pragma omp simd
            for (size_t i = 0; i < N; i++)
            {
                currentValue += WeightVector[i] * currentFeatureVector[i];
            }

            float nextValue = 0.0f;
            if (!nextState.isTerminal())
            {
                #pragma omp simd
                for (size_t i = 0; i < N; i++)
                {
                    nextValue += WeightVector[i] * nextFeaturesVector[i];
                }
            }

            #pragma omp simd
            for (size_t i = 0; i < N; i++)
            {
                WeightVector[i] += LearningRate * (nextState.getReward() + (DiscountFactor * nextValue) - currentValue) * currentFeatureVector[i];
            }
        }

        void updateWeightVectorNSarsa(const std::array<float, N>& currentFeatureVector, const std::array<float, N>& nextFeaturesVector,
            const State& nextState)
        {
            if (Visits.size() < n_Steps - 1)
            {
                Visits.push_back({ currentFeatureVector, nextState.getReward() });
            }
            else if (nextState.isTerminal() || Visits.size() == n_Steps - 1)
            {
                float currentValue = 0.0f;
                #pragma omp simd
                for (size_t i = 0; i < N; i++)
                {
                    currentValue += WeightVector[i] * currentFeatureVector[i];
                }

                float expectedRewards = 0.0f;
                float discount = 1.0f;
                for (size_t i = 0; i < Visits.size(); i++)
                {
                    expectedRewards += discount * Visits.at(i).Reward;
                    discount *= DiscountFactor;
                }

                float nextValue = 0.0f;
                if (!nextState.isTerminal())
                {
                    #pragma omp simd
                    for (size_t i = 0; i < N; i++)
                    {
                        nextValue += WeightVector[i] * nextFeaturesVector[i];
                    }
                }
                else
                {
                    nextValue = nextState.getReward();
                }
                nextValue = expectedRewards + (discount * nextValue);

                #pragma omp simd
                for (size_t i = 0; i < N; i++)
                {
                    WeightVector[i] += LearningRate * (nextValue - currentValue) * currentFeatureVector[i];
                }
                Visits.pop_front();
                Visits.emplace_back(currentFeatureVector, nextState.getReward());

                if (nextState.isTerminal())
                {
                    while (!Visits.empty())
                    {
                        updateWeightVectorWhenTerminal();
                    }
                }
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