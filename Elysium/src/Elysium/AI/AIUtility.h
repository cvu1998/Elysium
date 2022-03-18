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
            MEAN_SQUARED = 0
        };

        enum class Initializer
        {
            ZEROS = 0,
            RANDOM = 1,
            HE = 2
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

        float MeanSquareDerivative(float target, float prediction);
        float MeanSquareError(float target, float prediction);

        void softmax(const std::vector<float>& x, std::vector<float>& y, float beta = 1);
        void softmax(const Matrix& x, Matrix& y, float beta = 1);

        void softmin(const std::vector<float>& x, std::vector<float>& y, float beta = 1);
        void softmin(const Matrix& x, Matrix& y, float beta = 1);
    }
}