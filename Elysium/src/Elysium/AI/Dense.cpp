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
            m_Activation = std::bind(&AI::sigmoid, std::placeholders::_1);
            break;
        }
    }

    void Dense::forwardPass(const Matrix& inputs, Matrix& results)
    {
        if (Weights.Values.empty())
        {
            Weights = Matrix(m_Units, inputs.getWidth(), true);
            //Weights = Matrix(m_Units, inputs.getWidth() + 1, true);
            Utility::CreateRandomVector(Weights.Values, Weights.getWidth() * Weights.getHeight(), -1.0f, 1.0f);

            results = Matrix(inputs.getHeight(), m_Units);
        }

        Weights.print();
        for (size_t i = 0; i < inputs.getHeight(); ++i)
        {
            for (size_t a = 0; a < m_Units; ++a)
            {
                for (size_t b = 0; b < inputs.getWidth(); ++b)
                    results[{i, a}] += Weights[{a, b}] * inputs[{i, b}];
                //results[{i, a}] += Weights[{a, inputs.getWidth()}];

                results[{i, a}] = m_Activation(results[{i, a}]);
            }
        }
        results.print();
    }


    float Dense::calculateError(const Matrix& inputs, const Matrix& outputs,
        Matrix& results, Matrix& error)
    {
        if (Weights.Values.empty())
        {
            Weights = Matrix(m_Units, inputs.getWidth(), true);
            //Weights = Matrix(m_Units, inputs.getWidth() + 1, true);
            Utility::CreateRandomVector(Weights.Values, Weights.getWidth() * Weights.getHeight(), -1.0f, 1.0f);

            results = Matrix(inputs.getHeight(), m_Units);
            error = Matrix(results.getHeight(), results.getWidth());
        }

        Weights.print();
        float meanError = 0.0f;
        for (size_t i = 0; i < inputs.getHeight(); ++i)
        {
            for (size_t a = 0; a < m_Units; ++a)
            {
                for (size_t b = 0; b < inputs.getWidth(); ++b)
                    results[{i, a}] += Weights[{a, b}] * inputs[{i, b}];
                //results[{i, a}] += Weights[{a, inputs.getWidth()}];

                results[{i, a}] = m_Activation(results[{i, a}]);
                error[{i, a}] = outputs.Values[i] - results[{i, a}];
                meanError += fabs(error[{i, a}]);
            }
        }
        results.print();
        error.print();

        return meanError / (float)error.getHeight();
    }
}