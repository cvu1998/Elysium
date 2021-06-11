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

        virtual void forwardPass(const Matrix& inputs, 
            Matrix& results) = 0;
        virtual float calculateError(const Matrix& inputs, const Matrix& outputs, 
            Matrix& results, Matrix& error) = 0;
        virtual void calculateDelta(const Matrix& error, const Matrix& outputs,
            Matrix& delta) = 0;
        virtual void backwardPass(const Matrix& prevDelta, const Matrix& prevWeights, const Matrix& outputs,
            Matrix& delta) = 0;

    public:
        Matrix Weights;

        float LearningRate = 0.1f;

    protected:
        using MathFn = std::function<float(float)>;

        MathFn m_Activation;
        MathFn m_ActivationDerivative;
    };
}