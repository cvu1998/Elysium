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

        float AbsoluteDerivative(float correct, float prediction);
        float AbsoluteError(float value);
        float Mean(float value, size_t n);

        float MeanSquareDerivative(float correct, float prediction);
        float MeanSquareError(float value);

        void softmax(const std::vector<float>& x, std::vector<float>& y, float beta = 1);
        void softmax(const Matrix& x, Matrix& y, float beta = 1);

        void softmin(const std::vector<float>& x, std::vector<float>& y, float beta = 1);
        void softmin(const Matrix& x, Matrix& y, float beta = 1);
    }
}