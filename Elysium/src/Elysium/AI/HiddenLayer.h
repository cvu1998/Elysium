#pragma once

#include <functional>

#include "Elysium/AI/AIUtility.h"
#include "Elysium/AI/Matrix.h"

namespace Elysium
{
    class Model;

    class HiddenLayer
    {
        friend class Model;

    public:
        HiddenLayer() = default;
        HiddenLayer(AI::Activation activation);
        virtual ~HiddenLayer() = default;

    public:
        Matrix Weights;

    protected:
        using MathFn = std::function<float(float)>;
        using LossFn = std::function<float(float, float)>;
        using ScoreFn = std::function<float(float, size_t)>;

        void getActivation(MathFn& function);
        void getActivationDerivative(MathFn& function);
        void getLossAndScore(AI::Loss loss, LossFn& lossFn, ScoreFn& scoreFn);

        virtual bool forwardPass(const Matrix& inputs,
            Matrix& results, Matrix& activations) = 0;
        virtual float calculateError(const Matrix& inputs, const Matrix& outputs, AI::Loss lossFunction,
            Matrix& results, Matrix& activations, Matrix& error) = 0;
        virtual void calculateDelta(const Matrix& error, const Matrix& outputs, const Matrix& inputs,
            Matrix& delta) = 0;
        virtual void backwardPass(const Matrix& prevDelta, const Matrix& prevWeights, const Matrix& outputs, const Matrix& inputs,
            Matrix& delta) = 0;

    protected:
        float LearningRate = 0.1f;

        AI::Activation m_Activation = AI::Activation::LINEAR;
    };
}