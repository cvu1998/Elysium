#include "HiddenLayer.h"

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
}