#include "Dense.h"

#include <algorithm>
#include <execution>

#include "Elysium/Log.h"
#include "Elysium/Utility.h"

namespace Elysium
{
    Dense::Dense(size_t units, AI::Activation activation, AI::Initializer initializer, bool useBias) :
        HiddenLayer("Dense", units, activation, initializer, useBias)
    {
    }

    bool Dense::forward(const Matrix& inputs,
        Matrix& preActivations, Matrix& activations)
    {
        if (Weights.getWidth() != inputs.getWidth())
            return false;

        preActivations = Matrix(inputs.getHeight(), m_Size);
        activations = Matrix(inputs.getHeight(), m_Size);

        ActivationFn activationFn;
        getActivation(activationFn);

        for (size_t i = 0; i < inputs.getHeight(); ++i)
        {
            for (size_t a = 0; a < m_Size; ++a)
            {
                for (size_t b = 0; b < inputs.getWidth(); ++b) preActivations(i, a) += Weights(a, b) * inputs(i, b);
                preActivations(i, a) += Biases[a];

                activations(i, a) = activationFn(preActivations(i, a));
            }
        }
        return true;
    }


    float Dense::calculateLoss(const Matrix& inputs, const Matrix& targets, AI::Loss lossFunction,
        Matrix& preActivations, Matrix& activations, Matrix& dL_wrt_dO)
    {
        preActivations = Matrix(inputs.getHeight(), m_Size);
        activations = Matrix(inputs.getHeight(), m_Size);
        dL_wrt_dO = Matrix(preActivations.getHeight(), preActivations.getWidth());

        ActivationFn activationFn;
        getActivation(activationFn);

        LossFn lossFn;
        LossFnDerivative derivativeFn;
        getLoss(lossFunction, lossFn, derivativeFn);

        float loss = 0.0f;
        size_t batchSize = inputs.getHeight();
        for (size_t i = 0; i < batchSize; ++i)
        {
            for (size_t a = 0; a < m_Size; ++a)
            {
                for (size_t b = 0; b < inputs.getWidth(); ++b) preActivations(i, a) += Weights(a, b) * inputs(i, b);
                preActivations(i, a) += Biases[a];

                activations(i, a) = activationFn(preActivations(i, a));
                loss += lossFn(targets(i, a), activations(i, a));
                dL_wrt_dO(i, a) = derivativeFn(targets(i, a), activations(i, a));
            }
        }
        return loss / (float)batchSize;
    }

    void Dense::calculateOutputGradient(const Matrix& dL_wrt_dO, const Matrix& outputs, const Matrix& inputs,
        Matrix& dL_wrt_dH)
    {
        size_t batchSize = outputs.getHeight();
        dL_wrt_dH = Matrix(outputs.getHeight(), outputs.getWidth());

        ActivationFn activationDerivativeFn;
        getActivationDerivative(activationDerivativeFn);

        for (size_t i = 0; i < dL_wrt_dH.Values.size(); ++i) dL_wrt_dH.Values[i] = dL_wrt_dO.Values[i] * activationDerivativeFn(outputs.Values[i]);

        for (size_t a = 0; a < inputs.getWidth(); ++a)
        {
            for (size_t i = 0; i < m_Size; ++i)
            {
                float dL_wrt_dW = 0.0f;
                float dL_wrt_dB = 0.0f;
                for (size_t b = 0; b < inputs.getHeight(); ++b)
                {
                    dL_wrt_dW += dL_wrt_dH(b, i) * inputs(b, a);
                    dL_wrt_dB += dL_wrt_dH(b, i);
                }
                Weights(i, a) -= ((1.0f / batchSize) * dL_wrt_dW + (Regurlarization * Weights(i, a)) ) * LearningRate;

                if (m_Bias) Biases[i] -= (1.0f / batchSize) * dL_wrt_dB * LearningRate;
            }
        }
    }

    void Dense::backward(const Matrix& dL_wrt_dH_1, const Matrix& weights_1, const Matrix& preActivations, const Matrix& inputs,
        Matrix& dL_wrt_dH)
    {
        size_t batchSize = preActivations.getHeight();
        dL_wrt_dH = Matrix(preActivations.getHeight(), preActivations.getWidth());

        ActivationFn activationDerivativeFn;
        getActivationDerivative(activationDerivativeFn);

        for (size_t i = 0; i < dL_wrt_dH_1.getHeight(); ++i)
        {
            for (size_t a = 0; a < weights_1.getWidth(); ++a)
            {
                float dO_wrt_dH = activationDerivativeFn(preActivations(i, a));
                for (size_t b = 0; b < dL_wrt_dH_1.getWidth(); ++b) dL_wrt_dH(i, a) = dL_wrt_dH_1(i, b) * weights_1(b, a) * dO_wrt_dH;
            }
        }

        for (size_t a = 0; a < inputs.getWidth(); ++a)
        {
            for (size_t i = 0; i < m_Size; ++i)
            {
                float dL_wrt_dW = 0.0f;
                float dL_wrt_dB = 0.0f;
                for (size_t b = 0; b < inputs.getHeight(); ++b)
                {
                    dL_wrt_dW +=  dL_wrt_dH(b, i) * inputs(b, a );
                    dL_wrt_dB +=  dL_wrt_dH(b, i);
                }
                Weights(i, a) -= ((1.0f / batchSize) * dL_wrt_dW) * LearningRate;

                if (m_Bias) Biases[i] -= (1.0f / batchSize) * dL_wrt_dB * LearningRate;
            }
        }
    }
}