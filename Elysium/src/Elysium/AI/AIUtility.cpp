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

        float Tanh(float x)
        {
            return (glm::exp(x) - glm::exp(-x)) / (glm::exp(x) + glm::exp(-x));
        }

        float TanhDerivative(float x)
        {
            return 1 - (Tanh(x) * Tanh(x));
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

        float MeanSquareDerivative(float target, float prediction)
        {
            return -(target - prediction);
        }

        float MeanSquareError(float target, float prediction)
        {
            return 0.5f * (target  - prediction) * (target - prediction);
        }

        void softmax(const std::vector<float>& x, std::vector<float>& y, float beta)
        {
            float sum = 0.0f;
            for (float i : x)
                sum += glm::exp(beta * i);

            y.reserve(x.size());
            for (float i : x)
                y.emplace_back(glm::exp(beta * i) / sum);
        }

        void softmax(const Matrix& x, Matrix& y, float beta)
        {
            y = Matrix(x.getHeight(), x.getWidth());
            for (size_t i = 0; i < x.getHeight(); ++i)
            {
                float sum = 0.0f;
                for (size_t j = 0; j < x.getHeight(); ++j)
                    sum += glm::exp(beta * x(i, j));

                for (size_t j = 0; j < x.getHeight(); ++j)
                    y(i, j) = glm::exp(beta * x(i, j)) / sum;
            }
        }

        void softmin(const std::vector<float>& x, std::vector<float>& y, float beta)
        {
            float sum = 0.0f;
            for (float i : x)
                sum += glm::exp(-(beta * i));

            y.reserve(x.size());
            for (float i : x)
                y.emplace_back(glm::exp(-(beta * i)) / sum);
        }

        void softmin(const Matrix& x, Matrix& y, float beta)
        {
            y = Matrix(x.getHeight(), x.getWidth());
            for (size_t i = 0; i < x.getHeight(); ++i)
            {
                float sum = 0.0f;
                for (size_t j = 0; j < x.getHeight(); ++j)
                    sum += glm::exp(-(beta * x(i, j)));

                for (size_t j = 0; j < x.getHeight(); ++j)
                    y(i, j) = glm::exp(-(beta * x(i, j))) / sum;
            }
        }
    }
}