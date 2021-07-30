#include "AIUtility.h"

#include <glm/glm.hpp>

namespace Elysium
{
    namespace AI
    {
        float LeakySlope = 0.01f;

        float Step(float x)
        {
            return x >= 1.0f ? 1.0f : 0.0f;
        }

        float Sigmoid(float x)
        {
            return  1.0f / (1.0f + glm::exp(-x));
        }

        float SigmoidDerivative(float x)
        {
            return Sigmoid(x) * (1.0f - Sigmoid(x));
        }

        float Linear(float x)
        {
            return x;
        }

        float LinearDerivative(float x)
        {
            return 1.0f;
        }

        float Relu(float x)
        {
            return glm::max(0.0f * x, x);
        }

        float ReluDerivative(float x)
        {
            return x > 0.0f ? 1.0f : 0.0f;
        }

        float LeakyRelu(float x)
        {
            return glm::max(LeakySlope * x, x);
        }

        float LeakyReluDerivative(float x)
        {
            return x > 0.0f ? 1.0f : LeakySlope;
        }

        float Absolute(float correct, float prediction)
        {
            return correct - prediction;
        }

        float Mean(float value, size_t n)
        {
            return value / (float)n;
        }

        float MeanSquare(float correct, float prediction)
        {
            return correct * correct - prediction * prediction;
        }

        float RootMeanSquare(float value, size_t n)
        {
            return glm::sqrt(value / (2.0f * (float)n));
        }

        void softmax(const std::vector<float>& x, std::vector<float>& y)
        {
            float sum = 0.0f;
            for (float i : x)
                sum += glm::exp(i);

            y.reserve(x.size());
            for (float i : x)
                y.emplace_back(glm::exp(i) / sum);
        }

        void softmax(const Matrix& x, Matrix& y)
        {
            y = Matrix(x.getHeight(), x.getWidth());
            for (size_t i = 0; i < x.getHeight(); ++i)
            {
                float sum = 0.0f;
                for (size_t j = 0; j < x.getHeight(); ++j)
                    sum += glm::exp(x[{i, j}]);

                for (size_t j = 0; j < x.getHeight(); ++j)
                    y[{i, j}] = glm::exp(x[{i, j}]) / sum;
            }
        }
    }
}