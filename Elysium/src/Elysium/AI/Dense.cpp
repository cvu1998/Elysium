#include "Dense.h"

#include <algorithm>
#include <execution>

#include "Elysium/Log.h"
#include "Elysium/Utility.h"

namespace Elysium
{
    Dense::Dense(size_t units, AI::Activation activation) : m_Units(units)
    {
        switch (activation)
        {
        case AI::Activation::SIGMOID:
            m_Activation = std::bind(&AI::Sigmoid, std::placeholders::_1);
            m_ActivationDerivative = std::bind(&AI::SigmoidDerivative, std::placeholders::_1);
            break;
        default:
            m_Activation = std::bind(&AI::Sigmoid, std::placeholders::_1);
            m_ActivationDerivative = std::bind(&AI::SigmoidDerivative, std::placeholders::_1);
        }
    }

    void Dense::forwardPass(const Matrix& inputs, 
        Matrix& results)
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

    void Dense::calculateDelta(const Matrix& error, const Matrix& outputs, 
        Matrix& delta)
    {
        if (delta.Values.empty())
        {
            delta = Matrix(outputs.getHeight(), outputs.getWidth());
        }

        std::for_each(
            std::execution::par,
            outputs.Values.begin(),
            outputs.Values.end(),
            [&, idx = 0](float value) mutable
        {
            delta.Values[idx++] = error.Values[idx] * AI::SigmoidDerivative(value);
        });
        delta.print();
    }

    void Dense::backwardPass(const Matrix& prevDelta, const Matrix& prevWeights, const Matrix& outputs,
        Matrix& delta)
    {
        if (delta.Values.empty())
        {
            delta = Matrix(outputs.getHeight(), outputs.getWidth());
        }

        for (size_t i = 0; i < prevDelta.getHeight(); ++i)
        {
            for (size_t a = 0; a < m_Units; ++a)
            {
                for (size_t b = 0; b < prevDelta.getWidth(); ++b)
                    delta[{i, a}] = prevDelta[{i, b}] * prevWeights[{b, a}] * m_ActivationDerivative(outputs[{i, a}]);
            }
        }
        delta.print();
    }
}