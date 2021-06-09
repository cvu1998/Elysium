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
            m_Activation = std::bind(&AI::sigmoid, std::placeholders::_1);
            break;
        }
    }

    void Perceptron::fit(const Matrix& x, const Matrix& y, size_t epochs)
    {
        if (x.getHeight() != y.getHeight())
        {
            ELY_CORE_ERROR("Incomplete training samples!");
            return;
        }

        if (Weights.empty())
            Weights.resize(x.getWidth() + 1);
        
        for (size_t t = epochs; epochs > 0; --epochs)
        {
            for (size_t j = 0; j < x.getHeight(); ++j)
            {
                float result = 0.0f;
                for (size_t i = 0; i < x.getWidth(); ++i)
                    result += Weights[i] * x[{j, i}];
                result += Weights[x.getWidth()];

                result = m_Activation(result);
                float error = y.Values[j] - result;
                if (fabs(error) > 0.0f)
                {
                    for (size_t i = 0; i < x.getWidth(); ++i)
                        Weights[i] = Weights[i] + LearningRate * error * x[{j, i}];
                    Weights[x.getWidth()] = Weights[x.getWidth()] + LearningRate * error;
                }
            }
        }
    }

    void Perceptron::predict(const Matrix& x, std::vector<float>& results)
    {
        results.reserve(x.getHeight());
        for (size_t j = 0; j < x.getHeight(); ++j)
        {
            float result = 0.0f;
            for (size_t i = 0; i < x.getWidth(); ++i)
                result += Weights[i] * x[{j, i}];
            result += Weights[x.getWidth()];

            results.emplace_back(m_Activation(result));
        }
    }

    float Perceptron::score(const Matrix& x, const Matrix& y)
    {
        float meanAccuracy = 0.0f;
        for (size_t j = 0; j < x.getHeight(); ++j)
        {
            float result = 0.0f;
            for (size_t i = 0; i < x.getWidth(); ++i)
                result += Weights[i] * x[{j, i}];
            result += Weights[x.getWidth()];

            result = m_Activation(result);
            meanAccuracy += fabs(y.Values[j] - result);
        }

        return 1.0f - (meanAccuracy / (float)x.getHeight());
    }
}