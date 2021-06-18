#include "Model.h"

#include "Elysium/Log.h"

namespace Elysium
{
    Model::Model(size_t numberOfLayers)
    {
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
        const size_t last = m_Layers.size() - 1;
        const size_t epochsPoint = epochs > 2 ? (size_t)((float)epochs * 0.05f) : 1;

        for (epochs; epochs > 0; --epochs)
        {
            Matrix error;
            std::vector<Matrix> neurons(m_Layers.size());

            if (!m_Layers[0]->forwardPass(inputs, neurons[0]))
            {
                ELY_CORE_ERROR("Invalid input size!");
                return;
            }

            for (size_t i = 1; i < last; ++i)
                    m_Layers[i]->forwardPass(neurons[i - 1], neurons[i]);
            float meanError = m_Layers[last]->calculateError(neurons[last - 1], outputs, LossFunction, neurons[last], error);
            if (epochs % epochsPoint == 0)
                m_Summary.push_back({ epochs, meanError });

            Matrix currDelta;
            Matrix prevDelta;
            Matrix prevLayerWeights = m_Layers[last]->Weights;
            Matrix weights;

            m_Layers[last]->LearningRate = LearningRate;
            m_Layers[last]->calculateDelta(error, neurons[last], neurons[last - 1], currDelta);
            prevDelta = currDelta;
            for (int i = (int)last - 1; i > 0; --i)
            {
                weights = m_Layers[i]->Weights;

                m_Layers[i]->LearningRate = LearningRate;
                m_Layers[i]->backwardPass(prevDelta, prevLayerWeights, neurons[i], neurons[last - 1], currDelta);

                prevDelta = currDelta;
                prevLayerWeights = weights;
            }
            m_Layers[0]->LearningRate = LearningRate;
            m_Layers[0]->backwardPass(prevDelta, prevLayerWeights, neurons[0], inputs, currDelta);
        }
    }

    void Model::predict(const Matrix& inputs, Matrix& results)
    {
        const size_t last = m_Layers.size() - 1;

        std::vector<Matrix> neurons(last);

        if (!m_Layers[0]->forwardPass(inputs, neurons[0]))
        {
            ELY_CORE_ERROR("Invalid input size!");
            return;
        }

        for (size_t i = 1; i < last; ++i)
            m_Layers[i]->forwardPass(neurons[i - 1], neurons[i]);
        m_Layers[last]->forwardPass(neurons[last - 1], results);
    }

    float Model::score(const Matrix& inputs, const Matrix& outputs, Matrix& results)
    {
        const size_t last = m_Layers.size() - 1;

        Matrix error;
        std::vector<Matrix> neurons(last);

        if (!m_Layers[0]->forwardPass(inputs, neurons[0]))
        {
            ELY_CORE_ERROR("Invalid input size!");
            return 1.0f;
        }

        for (size_t i = 1; i < last; ++i)
            m_Layers[i]->forwardPass(neurons[i - 1], neurons[i]);
        return m_Layers[last]->calculateError(neurons[last - 1], outputs, LossFunction, results, error);
    }
}