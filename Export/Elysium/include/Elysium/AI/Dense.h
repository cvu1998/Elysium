#pragma once

#include "Elysium/AI/HiddenLayer.h"

namespace Elysium
{
    class Dense : public HiddenLayer
    {
    public:
        Dense(size_t units, AI::Activation activation = AI::Activation::SIGMOID);

        void forwardPass(const Matrix& inputs, Matrix& results) override;
        float calculateError(const Matrix& inputs, const Matrix& outputs,
            Matrix& results, Matrix& error)  override;

    private:
        size_t m_Units = 1;
    };
}