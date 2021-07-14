#pragma once

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
            FORECAST = 0,
            MEAN_SQUARE,
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

        float Forecast(float correct, float prediction);
        float Mean(float value, size_t n);

        float MeanSquare(float correct, float prediction);
        float RootMeanSquare(float value, size_t n);
    }
}