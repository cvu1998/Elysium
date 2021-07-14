#include "HiddenLayer.h"

#include <fstream>

#include "Elysium/Log.h"

namespace Elysium
{
    HiddenLayer::HiddenLayer(const char* layerName, AI::Activation activation, bool useBias) :
        m_LayerName(layerName),
        m_Activation(activation),
        m_Bias(useBias)
    {
    }

    void HiddenLayer::getActivation(MathFn& function)
    {
        switch (m_Activation)
        {
        case AI::Activation::LINEAR:
            function = std::bind(&AI::Linear, std::placeholders::_1);
            break;
        case AI::Activation::RELU:
            function = std::bind(&AI::Relu, std::placeholders::_1);
            break;
        case AI::Activation::LEAKY_RELU:
            function = std::bind(&AI::LeakyRelu, std::placeholders::_1);
            break;
        case AI::Activation::SIGMOID:
            function = std::bind(&AI::Sigmoid, std::placeholders::_1);
            break;
        }
    }

    void HiddenLayer::getActivationDerivative(MathFn& function)
    {
        switch (m_Activation)
        {
        case AI::Activation::LINEAR:
            function = std::bind(&AI::LinearDerivative, std::placeholders::_1);
            break;
        case AI::Activation::RELU:
            function = std::bind(&AI::ReluDerivative, std::placeholders::_1);
            break;
        case AI::Activation::LEAKY_RELU:
            function = std::bind(&AI::LeakyReluDerivative, std::placeholders::_1);
            break;
        case AI::Activation::SIGMOID:
            function = std::bind(&AI::SigmoidDerivative, std::placeholders::_1);
            break;
        }
    }

    void HiddenLayer::getLossAndScore(AI::Loss loss, LossFn& lossFn, ScoreFn& scoreFn)
    {
        switch (loss)
        {
        case AI::Loss::FORECAST:
            lossFn = std::bind(AI::Forecast, std::placeholders::_1, std::placeholders::_2);
            scoreFn = std::bind(AI::Mean, std::placeholders::_1, std::placeholders::_2);
            break;
        case AI::Loss::MEAN_SQUARE:
            lossFn = std::bind(AI::MeanSquare, std::placeholders::_1, std::placeholders::_2);
            scoreFn = std::bind(AI::RootMeanSquare, std::placeholders::_1, std::placeholders::_2);
            break;
        }
    }

    void HiddenLayer::summary() const
    {
        ELY_CORE_INFO("{0}:{2}{1}", 
            m_LayerName, 
            Weights.Values.size(), 
            std::string(strlen(m_LayerName) < s_BasePadding ? s_BasePadding - strlen(m_LayerName) : 0, ' '));
    }

    void HiddenLayer::saveWeightsAndBiases(const char* filename) const
    {
        std::ofstream file(filename);
        if (!file.is_open())
        {
            ELY_CORE_ERROR("Could not save weight and biases at {0}!", filename);
            return;
        }

        for (size_t i = 0; i < Weights.getHeight(); ++i)
        {
            for (size_t j = 0; j < Weights.getWidth(); ++j)
                file << "W(" << i << ", " << j << "): " << Weights[{i, j}] << '\n';
        }

        for (size_t i = 0; i < Biases.size(); ++i)
            file << "B(" << i << "): " << Biases[i] << "\n";
        file.close();
    }

    bool HiddenLayer::loadWeightsAndBiases(const char* filename)
    {
        std::ifstream file(filename);
        if (!file.is_open())
        {
            ELY_CORE_ERROR("Could not load weight and biases at {0}!", filename);
            return false;
        }

        std::string line;

        while (getline(file, line))
        {
            size_t i, j = 0;
            float value = 0.0f;
            switch (line[0])
            {
            case 'W':
                sscanf_s(line.c_str(), "W(%zu, %zu): %f", &i, &j, &value);
                Weights[{i, j}] = value;
                break;
            case 'B':
                sscanf_s(line.c_str(), "B(%zu): %f", &i, &value);
                Biases[i] = value;
                break;
            }
        }
        file.close();

        return true;
    }
}