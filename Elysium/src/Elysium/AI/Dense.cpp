#include "Dense.h"

#include "Elysium/Log.h"
#include "Elysium/Utility.h"

namespace Elysium
{
    Dense::Dense(size_t units, AI::Activation activation)
    {
        Weights.Height = units;
        switch (activation)
        {
        case AI::Activation::STEP:
            m_Activation = std::bind(&AI::step, std::placeholders::_1);
            break;
        case AI::Activation::SIGMOID:
            break;
        }
    }

    void Dense::fit(const Matrix& x, const Matrix& y)
    {
        if (x.Height != y.Height)
        {
            ELY_CORE_ERROR("Incomplete training samples!");
            return;
        }

        if (Weights.Values.empty())
        {
            //Weights.Width = x.Width + 1;
            Weights.Width = x.Width;
            Utility::CreateRandomVector(Weights.Values, Weights.Width * Weights.Height, -1.0f, 1.0f);
        }

        Weights.print();
        size_t epochs = 1;
        Matrix result(Weights.Height, x.Height);
        for (size_t t = epochs; epochs > 0; --epochs)
        {
            for (size_t j = 0; j < x.Height; ++j)
            {
                for (size_t k = 0; k < Weights.Height; ++k)
                {
                    for (size_t i = 0; i < x.Width; ++i)
                        result[{j, k}] += Weights[{k, i}] * x[{j, i}];
                    //result[{j, k}] += Weights[{k, Weights.Width - 1}];

                    result[{j, k}] = m_Activation(result[{j, k}]);
                }
            }
        }
        result.print();
        /*
        for (size_t t = epochs; epochs > 0; --epochs)
        {
            for (size_t j = 0; j < x.Height; ++j)
            {
                for (size_t k = 0; k < Weights.Height; ++k)
                {
                    float result = 0.0f;
                    for (size_t i = 0; i < x.Width; ++i)
                        result += Weights(k, i) * x(j, i);
                    //result += Weights(k, Weights.Width - 1);

                    result = m_Activation(result);
                    ELY_CORE_INFO("Result of unit {0} at step {1}: {2}", k, j, result);
                }
            }
        }
        */
    }
}