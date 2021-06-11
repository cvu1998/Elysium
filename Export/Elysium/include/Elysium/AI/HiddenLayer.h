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

        virtual void forwardPass(const Matrix& inputs, Matrix& results) = 0;
        virtual float calculateError(const Matrix& inputs, const Matrix& outputs, 
            Matrix& results, Matrix& error) = 0;

    public:
        Matrix Weights;

        float LearningRate = 0.1f;

    protected:
        using ActivationFn = std::function<float(float)>;

        ActivationFn m_Activation;
    };
}