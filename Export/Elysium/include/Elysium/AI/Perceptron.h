#pragma once

#include <functional>

#include "Elysium/AI/AIUtility.h"
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
        Perceptron(AI::Activation activation);

        void fit(const Matrix& x, const Matrix& y, size_t epochs = 1);

        void predict(const Matrix& x, std::vector<float>& results);
        float score(const Matrix& x, const Matrix& y);

    private:
        using ActivationFn = std::function<float(float)>;

        ActivationFn m_Activation = std::bind(&AI::step, std::placeholders::_1);
    };
}