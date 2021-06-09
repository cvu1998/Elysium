#pragma once

#include "Elysium/AI/HiddenLayer.h"

namespace Elysium
{
    class Dense : public HiddenLayer
    {
    public:
        Dense(size_t units, AI::Activation activation = AI::Activation::SIGMOID);

        void fit(const Matrix& x, const Matrix& y) override;

    private:
        size_t m_Units = 1;
    };
}