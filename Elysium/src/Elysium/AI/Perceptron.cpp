#include "Perceptron.h"

#include "Elysium/Log.h"

namespace Elysium
{
    Perceptron::Perceptron(AI::Activation activation)
    {
        switch (activation)
        {
        case AI::Activation::STEP:
            break;
        case AI::Activation::SIGMOID:
            m_Activation = std::bind(&AI::Sigmoid, std::placeholders::_1);
            break;
        }
    }

    void Perceptron::fit(const Matrix& inputs, const Matrix& outputs, size_t epochs)
    {
        if (inputs.getHeight() != inputs.getHeight())
        {
            ELY_CORE_ERROR("Incomplete training samples!");
            return;
        }

        if (Weights.empty())
            Weights.resize(inputs.getWidth() + 1);
        
        for (epochs; epochs > 0; --epochs)
        {
            for (size_t j = 0; j < inputs.getHeight(); ++j)
            {
                float result = 0.0f;
                for (size_t i = 0; i < inputs.getWidth(); ++i)
                    result += Weights[i] * inputs[{j, i}];
                result += Weights[inputs.getWidth()];

                result = m_Activation(result);
                float error = outputs.Values[j] - result;
                if (fabs(error) > 0.0f)
                {
                    for (size_t i = 0; i < inputs.getWidth(); ++i)
                        Weights[i] = Weights[i] + LearningRate * error * inputs[{j, i}];
                    Weights[inputs.getWidth()] = Weights[inputs.getWidth()] + LearningRate * error;
                }
            }
        }
    }

    void Perceptron::predict(const Matrix& inputs, std::vector<float>& results)
    {
        results.reserve(inputs.getHeight());
        for (size_t j = 0; j < inputs.getHeight(); ++j)
        {
            float result = 0.0f;
            for (size_t i = 0; i < inputs.getWidth(); ++i)
                result += Weights[i] * inputs[{j, i}];
            result += Weights[inputs.getWidth()];

            results.emplace_back(m_Activation(result));
        }
    }

    float Perceptron::score(const Matrix& inputs, const Matrix& outputs, std::vector<float>& results)
    {
        float meanAccuracy = 0.0f;
        results.reserve(inputs.getHeight());
        for (size_t j = 0; j < inputs.getHeight(); ++j)
        {
            float result = 0.0f;
            for (size_t i = 0; i < inputs.getWidth(); ++i)
                result += Weights[i] * inputs[{j, i}];
            result += Weights[inputs.getWidth()];

            result = m_Activation(result);
            meanAccuracy += fabs(outputs.Values[j] - result);
            results.emplace_back(result);
        }

        return 1.0f - (meanAccuracy / (float)outputs.getHeight());
    }
}