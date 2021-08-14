#include "HiddenLayer.h"

#include <fstream>

#include <glm/glm.hpp>

#include "Elysium/Log.h"
#include "Elysium/Random.h"
#include "Elysium/Utility.h"

namespace Elysium
{
    HiddenLayer::HiddenLayer(const char* layerName, size_t size, AI::Activation activation, AI::Initializer initializer, bool useBias) :
        m_Size(size),
        m_LayerName(layerName),
        m_Activation(activation),
        m_Initializer(initializer),
        m_Bias(useBias)
    {
    }

    void HiddenLayer::initWeightAndBiases(size_t inputSize)
    {
        Random::Init();

        Weights = Matrix(m_Size, inputSize, true);
        switch (m_Initializer)
        {
        case AI::Initializer::ZEROS:
            Weights.Values.resize(m_Size * inputSize, 0.0f);
            break;
        case AI::Initializer::RANDOM:
            Utility::CreateRandomVector(Weights.Values, Weights.getWidth() * Weights.getHeight(), -1.0f, 1.0f);
            break;
        case AI::Initializer::HE:
            std::mt19937 random;
            std::normal_distribution<float> distribution(0.0f, 1.0f);

            random.seed(std::random_device()());
            float scaleFactor = glm::sqrt(2.0f / (float)inputSize);
            Weights.Values.reserve(Weights.getWidth() * Weights.getHeight());
            for (size_t i = 0; i < Weights.getWidth() * Weights.getHeight(); ++i)
                Weights.Values.emplace_back(distribution(random) * scaleFactor);
            break;
        }

        Biases.resize(m_Size);
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
        case AI::Loss::MEAN_ABSOLUTE:
            lossFn = std::bind(AI::Absolute, std::placeholders::_1, std::placeholders::_2);
            scoreFn = std::bind(AI::Mean, std::placeholders::_1, std::placeholders::_2);
            break;
        case AI::Loss::MEAN_SQUARED:
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
                file << "W(" << i << ", " << j << "): " << Weights(i, j) << '\n';
        }

        for (size_t i = 0; i < Biases.size(); ++i)
            file << "B(" << i << "): " << Biases[i] << "\n";
        file.close();
    }

    bool HiddenLayer::loadWeightsAndBiases(const char* filename)
    {
        std::ifstream file(filename);
        if (!file.is_open())
            return false;

        std::string line;

        while (getline(file, line))
        {
            size_t i, j = 0;
            float value = 0.0f;
            switch (line[0])
            {
            case 'W':
                sscanf_s(line.c_str(), "W(%zu, %zu): %f", &i, &j, &value);
                Weights(i, j) = value;
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