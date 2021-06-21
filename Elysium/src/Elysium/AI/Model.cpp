#include "Model.h"

#include "Elysium/Log.h"

namespace Elysium
{
    Model::Model(size_t numberOfLayers)
    {
        if (numberOfLayers < 2)
        {
            ELY_ERROR("Model must have 2 or more layers!");
            return;
        }
        m_Valid = true;
        m_Layers.reserve(numberOfLayers);
    }

    Model::~Model()
    {
        for (HiddenLayer* layer : m_Layers)
            delete layer;
        m_Layers.clear();
    }

    void Model::fit(const Matrix& inputs, const Matrix& outputs, size_t epochs)
    {
        if (!m_Valid)
            return;

        const size_t last = m_Layers.size() - 1;
        const size_t epochsPoint = epochs > 2 ? (size_t)((float)epochs * 0.05f) : 1;

        for (epochs; epochs > 0; --epochs)
        {
            Matrix error;
            std::vector<Matrix> neurons(m_Layers.size());
            std::vector<Matrix> activations(m_Layers.size());

            if (!m_Layers[0]->forwardPass(inputs, neurons[0], activations[0]))
            {
                ELY_CORE_ERROR("Invalid input size!");
                return;
            }
            m_Trained = true;

            for (size_t i = 1; i < last; ++i)
                    m_Layers[i]->forwardPass(activations[i - 1], neurons[i], activations[i]);
            float meanError = m_Layers[last]->calculateError(
                activations[last - 1],
                outputs, 
                LossFunction, 
                neurons[last], 
                activations[last], 
                error);
            if (epochs % epochsPoint == 0)
                m_Summary.push_back({ epochs, meanError });

            Matrix currDelta;
            Matrix prevDelta;
            Matrix prevLayerWeights = m_Layers[last]->Weights;
            Matrix weights;

            m_Layers[last]->LearningRate = LearningRate;
            m_Layers[last]->calculateDelta(error, neurons[last], activations[last - 1], currDelta);
            prevDelta = currDelta;
            for (int i = (int)last - 1; i > 0; --i)
            {
                weights = m_Layers[i]->Weights;

                m_Layers[i]->LearningRate = LearningRate;
                m_Layers[i]->backwardPass(prevDelta, prevLayerWeights, neurons[i], activations[last - 1], currDelta);

                prevDelta = currDelta;
                prevLayerWeights = weights;
            }
            m_Layers[0]->LearningRate = LearningRate;
            m_Layers[0]->backwardPass(prevDelta, prevLayerWeights, neurons[0], inputs, currDelta);
        }
    }

    void Model::predict(const Matrix& inputs, Matrix& results)
    {
        if (!m_Trained || !m_Valid)
            return;

        const size_t last = m_Layers.size() - 1;

        Matrix lastNeuron;
        std::vector<Matrix> neurons(last);
        std::vector<Matrix> activations(last);

        if (!m_Layers[0]->forwardPass(inputs, neurons[0], activations[0]))
        {
            ELY_CORE_ERROR("Invalid input size!");
            return;
        }

        for (size_t i = 1; i < last; ++i)
            m_Layers[i]->forwardPass(activations[i - 1], neurons[i], activations[i]);
        m_Layers[last]->forwardPass(activations[last - 1], lastNeuron, results);
    }

    float Model::score(const Matrix& inputs, const Matrix& outputs, Matrix& results)
    {
        if (!m_Trained || !m_Valid)
            return 1.0f;

        const size_t last = m_Layers.size() - 1;

        Matrix error;
        Matrix lastNeuron;
        std::vector<Matrix> neurons(last);
        std::vector<Matrix> activations(last);

        if (!m_Layers[0]->forwardPass(inputs, neurons[0], activations[0]))
        {
            ELY_CORE_ERROR("Invalid input size!");
            return 1.0f;
        }

        for (size_t i = 1; i < last; ++i)
            m_Layers[i]->forwardPass(activations[i - 1], neurons[i], activations[0]);
        return m_Layers[last]->calculateError(activations[last - 1], outputs, LossFunction, lastNeuron, results, error);
    }
}