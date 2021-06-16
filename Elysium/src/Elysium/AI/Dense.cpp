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
            Utility::CreateRandomVector(Weights.Values, Weights.getWidth() * Weights.getHeight(), -1.0f, 1.0f);
        }

        results = Matrix(inputs.getHeight(), m_Units);

        ELY_CORE_INFO("WEIGHTS");
        Weights.print();
        for (size_t i = 0; i < inputs.getHeight(); ++i)
        {
            for (size_t a = 0; a < m_Units; ++a)
            {
                for (size_t b = 0; b < Weights.getWidth(); ++b)
                    results[{i, a}] += Weights[{a, b}] * inputs[{i, b}];

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
            Utility::CreateRandomVector(Weights.Values, Weights.getWidth() * Weights.getHeight(), -1.0f, 1.0f);
        }

        results = Matrix(inputs.getHeight(), m_Units);
        error = Matrix(results.getHeight(), results.getWidth());

        ELY_CORE_INFO("WEIGHTS");
        Weights.print();
        float meanError = 0.0f;
        for (size_t i = 0; i < inputs.getHeight(); ++i)
        {
            for (size_t a = 0; a < m_Units; ++a)
            {
                for (size_t b = 0; b < Weights.getWidth(); ++b)
                    results[{i, a}] += Weights[{a, b}] * inputs[{i, b}];

                results[{i, a}] = m_Activation(results[{i, a}]);
                error[{i, a}] = outputs.Values[i] - results[{i, a}];
                meanError += fabs(error[{i, a}]);
            }
        }
        results.print();
        error.print();

        return meanError / (float)error.getHeight();
    }

    void Dense::calculateDelta(const Matrix& error, const Matrix& outputs, const Matrix& inputs,
        Matrix& delta)
    {
        delta = Matrix(outputs.getHeight(), outputs.getWidth());
        std::for_each(
            std::execution::par,
            outputs.Values.begin(),
            outputs.Values.end(),
            [&, idx = 0](float value) mutable
        {
            delta.Values[idx++] = error.Values[idx] * m_ActivationDerivative(value);
        });
        delta.print();

        for (size_t a = 0; a < inputs.getWidth(); ++a)
        {
            for (size_t i = 0; i < m_Units; ++i)
            {
                float step = 0.0f;
                for (size_t b = 0; b < inputs.getHeight(); ++b)
                    step += delta[{b, i}] * inputs[{b, a}] * LearningRate;
                Weights[{i, a}] += step;
            }
        }
        ELY_CORE_INFO("WEIGHTS AFTER UPDATE");
        Weights.print();
    }

    void Dense::backwardPass(const Matrix& prevDelta, const Matrix& prevWeights, const Matrix& outputs, const Matrix& inputs,
        Matrix& delta)
    {
        delta = Matrix(outputs.getHeight(), outputs.getWidth());
        for (size_t i = 0; i < prevDelta.getHeight(); ++i)
        {
            for (size_t a = 0; a < prevWeights.getWidth(); ++a)
            {
                for (size_t b = 0; b < prevDelta.getWidth(); ++b)
                    delta[{i, a}] = prevDelta[{i, b}] * prevWeights[{b, a}] * m_ActivationDerivative(outputs[{i, a}]);
            }
        }
        delta.print();

        for (size_t a = 0; a < inputs.getWidth(); ++a)
        {
            for (size_t i = 0; i < m_Units; ++i)
            {
                float step = 0.0f;
                for (size_t b = 0; b < inputs.getHeight(); ++b)
                    step += delta[{b, i}] * inputs[{b, a}] * LearningRate;
                Weights[{i, a}] += step;
            }
        }
        ELY_CORE_INFO("WEIGHTS AFTER UPDATE");
        Weights.print();
    }
}