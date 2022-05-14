#pragma once

#include "Elysium/AI/HiddenLayer.h"

namespace Elysium
{
    class Dense : public HiddenLayer
    {
    public:
        Dense(size_t units, 
            AI::Activation activation = AI::Activation::LINEAR, 
            AI::Initializer initializer = AI::Initializer::RANDOM, 
            bool useBias = true);

    protected:
        bool forward(const Matrix& inputs, Matrix& preActivations, Matrix& activations) override;

        float calculateLoss(const Matrix& inputs, const Matrix& targets, AI::Loss lossFunction,
            Matrix& preActivations, Matrix& activations, Matrix& gradient)  override;

        virtual void calculateOutputGradient(const Matrix& dL_wrt_dO, const Matrix& outputs, const Matrix& inputs,
            Matrix& dL_wrt_dH) override;

        virtual void backward(const Matrix& dL_wrt_dH_1, const Matrix& weights_1, const Matrix& preActivations, const Matrix& inputs,
            Matrix& dL_wrt_dH) override;
    };
}