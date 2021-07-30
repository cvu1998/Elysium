#pragma once

#include "Elysium/AI/Matrix.h"

namespace Elysium
{
    namespace AI
    {
        enum class Activation
        {
            STEP = 0,
            LINEAR,
            RELU,
            LEAKY_RELU,
            SIGMOID
        };

        enum class Loss
        {
            MEAN_ABSOLUTE = 0,
            MEAN_SQUARED,
        };

        extern float LeakySlope;

        float Step(float x);

        float Sigmoid(float x);
        float SigmoidDerivative(float x);

        float Linear(float x);
        float LinearDerivative(float x);

        float Relu(float x);
        float ReluDerivative(float x);

        float LeakyRelu(float x);
        float LeakyReluDerivative(float x);

        float Absolute(float correct, float prediction);
        float Mean(float value, size_t n);

        float MeanSquare(float correct, float prediction);
        float RootMeanSquare(float value, size_t n);

        void softmax(const std::vector<float>& x, std::vector<float>& y);
        void softmax(const Matrix& x, Matrix& y);
    }
}