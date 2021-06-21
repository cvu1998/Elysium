#include "HiddenLayer.h"

namespace Elysium
{
    HiddenLayer::HiddenLayer(AI::Activation activation) : m_Activation(activation)
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
}