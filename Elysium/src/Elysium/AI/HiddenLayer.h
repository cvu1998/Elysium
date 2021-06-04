#pragma once

#include <functional>

#include "Elysium/AI/AIUtility.h"
#include "Elysium/AI/Matrix.h"

namespace Elysium
{
    class HiddenLayer
    {
    public:
        virtual ~HiddenLayer() = default;

        virtual void fit(const Matrix& x, const Matrix& y) = 0;

    public:
        Matrix Weights;

        float LearningRate = 0.1f;

    protected:
        using ActivationFn = std::function<float(float)>;

        ActivationFn m_Activation = std::bind(&AI::sigmoid, std::placeholders::_1);
    };
}