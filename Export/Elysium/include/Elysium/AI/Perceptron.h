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

        void fit(const Matrix& inputs, const Matrix& outputs, size_t epochs = 1);

        void predict(const Matrix& inputs, std::vector<float>& results);
        float score(const Matrix& inputs, const Matrix& outputs, std::vector<float>& results);

    private:
        using ActivationFn = std::function<float(float)>;

        ActivationFn m_Activation = std::bind(&AI::Step, std::placeholders::_1);
    };
}