#pragma once

#include <functional>

#include "Elysium/AI/AIUtility.h"
#include "Elysium/AI/Matrix.h"

namespace Elysium
{
    class Model;

    class HiddenLayer
    {
        friend class Model;

    public:
        HiddenLayer
        (
            const char* layerName, 
            size_t size,
            AI::Activation activation,
            AI::Initializer initializer,
            bool useBias
        );
        virtual ~HiddenLayer() = default;

    public:
        Matrix Weights;
        std::vector<float> Biases;

    protected:
        using ActivationFn = std::function<float(float)>;
        using LossFn = std::function<float(float, float)>;
        using LossFnDerivative = std::function<float(float, float)>;

        virtual void initWeightAndBiases(size_t inputSize) final;

        void getActivation(ActivationFn& function);
        void getActivationDerivative(ActivationFn& function);
        void getLoss(AI::Loss loss, LossFn& lossFn, LossFnDerivative& derivativeFn);

        virtual bool forward(const Matrix& inputs, Matrix& preActivations, Matrix& activations) = 0;

        virtual float calculateLoss(const Matrix& inputs, const Matrix& targets, AI::Loss lossFunction,
            Matrix& preActivations, Matrix& activations, Matrix& error) = 0;

        virtual void calculateOutputGradient(const Matrix& dL_wrt_dO, const Matrix& outputs, const Matrix& inputs,
            Matrix& dL_wrt_dH) = 0;

        virtual void backward(const Matrix& dL_wrt_dH_1, const Matrix& weights_1, const Matrix& preActivations, const Matrix& inputs,
            Matrix& dL_wrt_dH) = 0;

        void summary() const;

        void saveWeightsAndBiases(const char* filename) const;
        bool loadWeightsAndBiases(const char* filename);

    protected:
        float LearningRate = 0.1f;
        size_t m_Size = 1;

        AI::Activation m_Activation = AI::Activation::LINEAR;
        AI::Initializer m_Initializer = AI::Initializer::RANDOM;
        bool m_Bias = true;

        const char* m_LayerName = nullptr;
        static constexpr size_t s_BasePadding = 16;
    };
}