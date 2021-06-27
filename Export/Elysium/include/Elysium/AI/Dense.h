#pragma once

#include "Elysium/AI/HiddenLayer.h"

namespace Elysium
{
    class Dense : public HiddenLayer
    {
    public:
        Dense(size_t units, AI::Activation activation = AI::Activation::LINEAR, bool useBias = true);

    protected:
        bool forwardPass(const Matrix& inputs, 
            Matrix& results, Matrix& activations) override;
        float calculateError(const Matrix& inputs, const Matrix& outputs, AI::Loss lossFunction,
            Matrix& results, Matrix& activations, Matrix& error)  override;
        virtual void calculateDelta(const Matrix& error, const Matrix& outputs, const Matrix& inputs,
            Matrix& delta) override;
        virtual void backwardPass(const Matrix& prevDelta, const Matrix& prevWeights, const Matrix& outputs, const Matrix& inputs,
            Matrix& delta) override;

    private:
        size_t m_Units = 1;
    };
}