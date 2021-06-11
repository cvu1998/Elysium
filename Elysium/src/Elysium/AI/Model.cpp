#include "Model.h"

#include "Elysium/Log.h"

namespace Elysium
{
    Model::Model(size_t numberOfLayers)
    {
        m_Layers.reserve(numberOfLayers);
        m_Neurons.resize(numberOfLayers);
        m_Deltas.resize(numberOfLayers);
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

        for (epochs; epochs > 0; --epochs)
        {
            float meanError = 0.0f;
            for (size_t i = 0; i < last; ++i)
            {
                switch (i)
                {
                case 0:
                    m_Layers[i]->forwardPass(inputs, m_Neurons[i]);
                    break;
                default:
                    m_Layers[i]->forwardPass(m_Neurons[i - 1], m_Neurons[i]);
                }
            }
            meanError = m_Layers[last]->calculateError(m_Neurons[last - 1], outputs, m_Neurons[last], m_Error);
            ELY_CORE_INFO("Mean Error: {0}", meanError);

            m_Layers[last]->calculateDelta(m_Error, m_Neurons[last], m_Deltas[last]);
            for (int i = (int)last - 1; i >= 0; --i)
                m_Layers[i]->backwardPass(m_Deltas[i + 1], m_Layers[i + 1]->Weights, m_Neurons[i], m_Deltas[i]);
        }
    }

    void Model::summary()
    {
    }
}