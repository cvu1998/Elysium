#include "Dense.h"

#include "Elysium/Log.h"
#include "Elysium/Utility.h"

namespace Elysium
{
    Dense::Dense(size_t units, AI::Activation activation) : m_Units(units)
    {
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
        if (x.getHeight() != y.getHeight())
        {
            ELY_CORE_ERROR("Incomplete training samples!");
            return;
        }

        if (Weights.Values.empty())
        {
            Weights = Matrix(m_Units, x.getWidth(), true);
            //Weights = Matrix(m_Units, x.getWidth() + 1, true);
            Utility::CreateRandomVector(Weights.Values, Weights.getWidth() * Weights.getHeight(), -1.0f, 1.0f);
        }

        Weights.print();
        size_t epochs = 1;
        Matrix results(x.getHeight(), m_Units);
        for (size_t t = epochs; epochs > 0; --epochs)
        {
            for (size_t i = 0; i < x.getHeight(); ++i)
            {
                for (size_t a = 0; a < m_Units; ++a)
                {
                    for (size_t b = 0; b < x.getWidth(); ++b)
                        results[{i, a}] += Weights[{a, b}] * x[{i, b}];
                    //results[{i, a}] += Weights[{a, x.getWidth()}];

                    results[{i, a}] = m_Activation(results[{i, a}]);
                }
            }
        }
        results.print();

        //----- SECOND DENSE LAYER INSIDE MODEL -----//
        ELY_CORE_INFO("---Second Dense Layer---");

        size_t units = 1;
        Matrix secondWeights(units, results.getWidth(), true);
        Utility::CreateRandomVector(secondWeights.Values, secondWeights.getWidth() * secondWeights.getHeight(), -1.0f, 1.0f);
        secondWeights.print();

        epochs = 1;
        Matrix result(results.getHeight(), units);
        for (size_t t = epochs; epochs > 0; --epochs)
        {
            for (size_t i = 0; i < results.getHeight(); ++i)
            {
                for (size_t a = 0; a < units; ++a)
                {
                    for (size_t b = 0; b < results.getWidth(); ++b)
                        result[{i, a}] += secondWeights[{a, b}] * results[{i, b}];
                    //results[{i, a}] += Weights[{a, x.getWidth()}];

                    result[{i, a}] = m_Activation(result[{i, a}]);
                }
            }
        }
        result.print();

    }
}