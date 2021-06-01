#include "Perceptron.h"

#include "Elysium/Log.h"
#include "Elysium/Utility.h"

namespace Elysium
{
    void Perceptron::fit(const Matrix& x, const Matrix& y, size_t epochs)
    {
        if (x.Height != y.Height)
        {
            ELY_CORE_ERROR("Incomplete training samples!");
            return;
        }

        if (Weights.empty())
            Weights.resize(x.Width + 1);
        
        for (size_t t = epochs; epochs > 0; --epochs)
        {
            for (size_t j = 0; j < x.Height; ++j)
            {
                float result = 0.0f;
                for (size_t i = 0; i < x.Width; ++i)
                    result += Weights[i] * x(j, i);
                result += Weights[x.Width];

                result = step(result);
                float error = y.Values[j] - result;
                if (fabs(error) > 0.0f)
                {
                    for (size_t i = 0; i < x.Width; ++i)
                        Weights[i] = Weights[i] + LearningRate * error * x(j, i);
                    Weights[x.Width] = Weights[x.Width] + LearningRate * error;
                }
            }
        }
    }

    void Perceptron::predict(const Matrix& x, std::vector<float>& results)
    {
        results.reserve(x.Height);
        for (size_t j = 0; j < x.Height; ++j)
        {
            float result = 0.0f;
            for (size_t i = 0; i < x.Width; ++i)
                result += Weights[i] * x(j, i);
            result += Weights[x.Width];

            results.emplace_back(step(result));
        }
    }

    float Perceptron::score(const Matrix& x, const Matrix& y)
    {
        float meanAccuracy = 0.0f;
        for (size_t j = 0; j < x.Height; ++j)
        {
            float result = 0.0f;
            for (size_t i = 0; i < x.Width; ++i)
                result += Weights[i] * x(j, i);
            result += Weights[x.Width];

            result = step(result);
            meanAccuracy += fabs(y.Values[j] - result);
        }

        return 1.0f - (meanAccuracy / (float)x.Height);
    }

    float Perceptron::step(float x)
    {
        return x >= 1.0f ? 1.0f : 0.0f;
    }
}