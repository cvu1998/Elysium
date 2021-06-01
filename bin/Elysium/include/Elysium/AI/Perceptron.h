#pragma once

#include "Elysium/AI/Matrix.h"

namespace Elysium
{
    class Perceptron final
    {
    public:
        std::vector<float> Weights;

        float LearningRate = 0.1f;

    public:
        Perceptron() = default;

        void fit(const Matrix& x, const Matrix& y, size_t epochs = 1);
        void predict(const Matrix& x, std::vector<float>& results);

    private:
        float step(float x);
    };
}