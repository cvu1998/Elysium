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
        const size_t epochsPoint = (size_t)((float)epochs * 0.05f) + 1;

        for (epochs; epochs > 0; --epochs)
        {
            std::vector<Matrix> neurons(m_Layers.size());

            m_Layers[0]->forwardPass(inputs, neurons[0]);
            for (size_t i = 1; i < last; ++i)
                    m_Layers[i]->forwardPass(neurons[i - 1], neurons[i]);
            Matrix error;
            float meanError = m_Layers[last]->calculateError(neurons[last - 1], outputs, neurons[last], error);
            if (epochs % epochsPoint == 0)
                m_Summary.push_back({ epochs, meanError });

            Matrix currDelta;
            Matrix prevDelta;
            Matrix prevLayerWeights = m_Layers[last]->Weights;
            Matrix weights;

            m_Layers[last]->LearningRate = LearningRate;
            m_Layers[last]->calculateDelta(error, neurons[last], neurons[last - 1], currDelta);
            prevDelta = currDelta;
            for (int i = (int)last - 1; i >= 0; --i)
            {
                weights = m_Layers[i]->Weights;

                m_Layers[i]->LearningRate = LearningRate;
                switch (i)
                {
                case 0:
                    m_Layers[i]->backwardPass(prevDelta, prevLayerWeights, neurons[i], inputs, currDelta);
                    break;
                default:
                    m_Layers[i]->backwardPass(prevDelta, prevLayerWeights, neurons[i], neurons[last - 1], currDelta);
                }
                prevDelta = currDelta;
                prevLayerWeights = weights;
            }
        }
    }

    void Model::predict(const Matrix& inputs, Matrix& results)
    {
        const size_t last = m_Layers.size() - 1;

        std::vector<Matrix> neurons(last);

        m_Layers[0]->forwardPass(inputs, neurons[0]);
        for (size_t i = 0; i < last; ++i)
            m_Layers[i]->forwardPass(neurons[i - 1], neurons[i]);
        m_Layers[last]->forwardPass(neurons[last - 1], results);
    }
}