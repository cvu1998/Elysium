#include "Dense.h"

#include <algorithm>
#include <execution>

#include "Elysium/Log.h"
#include "Elysium/Utility.h"

namespace Elysium
{
    Dense::Dense(size_t units, AI::Activation activation) : 
        HiddenLayer(activation),
        m_Units(units)
    {
    }

    bool Dense::forwardPass(const Matrix& inputs, 
        Matrix& results, Matrix& activations)
    {
        if (Weights.Values.empty())
        {
            Weights = Matrix(m_Units, inputs.getWidth(), true);
            Utility::CreateRandomVector(Weights.Values, Weights.getWidth() * Weights.getHeight(), -1.0f, 1.0f);
        }

        if (Weights.getWidth() != inputs.getWidth())
            return false;

        results = Matrix(inputs.getHeight(), m_Units);
        activations = Matrix(inputs.getHeight(), m_Units);

        MathFn activationFn;
        getActivation(activationFn);

        for (size_t i = 0; i < inputs.getHeight(); ++i)
        {
            for (size_t a = 0; a < m_Units; ++a)
            {
                for (size_t b = 0; b < inputs.getWidth(); ++b)
                    results[{i, a}] += Weights[{a, b}] * inputs[{i, b}];

                activations[{i, a}] = activationFn(results[{i, a}]);
            }
        }
        return true;
    }


    float Dense::calculateError(const Matrix& inputs, const Matrix& outputs, AI::Loss lossFunction,
        Matrix& results, Matrix& activations, Matrix& error)
    {
        if (Weights.Values.empty())
        {
            Weights = Matrix(m_Units, inputs.getWidth(), true);
            Utility::CreateRandomVector(Weights.Values, Weights.getWidth() * Weights.getHeight(), -1.0f, 1.0f);
        }

        results = Matrix(inputs.getHeight(), m_Units);
        activations = Matrix(inputs.getHeight(), m_Units);
        error = Matrix(results.getHeight(), results.getWidth());

        MathFn activationFn;
        getActivation(activationFn);

        LossFn lossFn;
        ScoreFn scoreFn;
        getLossAndScore(lossFunction, lossFn, scoreFn);

        float meanError = 0.0f;
        for (size_t i = 0; i < inputs.getHeight(); ++i)
        {
            for (size_t a = 0; a < m_Units; ++a)
            {
                for (size_t b = 0; b < inputs.getWidth(); ++b)
                    results[{i, a}] += Weights[{a, b}] * inputs[{i, b}];

                activations[{i, a}] = activationFn(results[{i, a}]);
                error[{i, a}] = lossFn(outputs[{i, a}], activations[{i, a}]);
                meanError += fabs(error[{i, a}]);
            }
        }
        return scoreFn(meanError, error.getHeight());
    }

    void Dense::calculateDelta(const Matrix& error, const Matrix& outputs, const Matrix& inputs,
        Matrix& delta)
    {
        delta = Matrix(outputs.getHeight(), outputs.getWidth());

        MathFn activationDerivativeFn;
        getActivationDerivative(activationDerivativeFn);

        for (size_t i = 0; i < delta.Values.size(); ++i)
            delta.Values[i] = error.Values[i] * activationDerivativeFn(outputs.Values[i]);

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
        //ELY_CORE_INFO("WEIGHTS AFTER UPDATE");
        //Weights.print();
    }

    void Dense::backwardPass(const Matrix& prevDelta, const Matrix& prevWeights, const Matrix& outputs, const Matrix& inputs,
        Matrix& delta)
    {
        delta = Matrix(outputs.getHeight(), outputs.getWidth());

        MathFn activationDerivativeFn;
        getActivationDerivative(activationDerivativeFn);

        for (size_t i = 0; i < prevDelta.getHeight(); ++i)
        {
            for (size_t a = 0; a < prevWeights.getWidth(); ++a)
            {
                for (size_t b = 0; b < prevDelta.getWidth(); ++b)
                    delta[{i, a}] = prevDelta[{i, b}] * prevWeights[{b, a}] * activationDerivativeFn(outputs[{i, a}]);
            }
        }

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
        //ELY_CORE_INFO("WEIGHTS AFTER UPDATE");
        //Weights.print();
    }
}