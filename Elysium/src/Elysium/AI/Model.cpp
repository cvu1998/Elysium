#include "Model.h"

#include "Elysium/Log.h"

namespace Elysium
{
    Model::~Model()
    {
        for (HiddenLayer* layer : m_Layers)
            delete layer;
        m_Layers.clear();
    }

    void Model::summary() const
    {
        ELY_CORE_INFO("{0}:    {1}", "Layer (Type)", "Param #");
        for (const auto& layer : m_Layers)
            layer->summary();

        ELY_CORE_INFO("Training Summary Report");
        for (const auto& p : m_TrainingSummary)
            ELY_CORE_INFO("Epoch: {0}, Error: {1}", p.Epoch, p.MeanError);
    }

    void Model::fit(const Matrix& inputs, const Matrix& outputs, size_t epochs, size_t batchSize)
    {
        if (!m_Valid)
            return;

        const size_t last = m_Layers.size() - 1;
        const size_t epochsPoint = epochs > 20 ? (size_t)((float)epochs * 0.05f) : 1;

        for (size_t epoch = 0; epoch < epochs; ++epoch)
        {
            size_t batchBegin = 0;
            size_t batchEnd = batchSize < inputs.getHeight() ? batchSize : 0;
            bool end = false;
            while (!end)
            {
                end = batchEnd == 0;
                Matrix input = Matrix::Slice(inputs, batchBegin, batchEnd, 0, 0);
                Matrix output = Matrix::Slice(outputs, batchBegin, batchEnd, 0, 0);

                Matrix error;
                std::vector<Matrix> neurons(m_Layers.size());
                std::vector<Matrix> activations(m_Layers.size());

                if (!m_Layers[0]->forwardPass(input, neurons[0], activations[0]))
                {
                    ELY_CORE_ERROR("Invalid input size!");
                    return;
                }
                m_Trained = true;

                for (size_t i = 1; i < last; ++i)
                    m_Layers[i]->forwardPass(activations[i - 1], neurons[i], activations[i]);
                float meanError = m_Layers[last]->calculateError(
                    activations[last - 1],
                    output,
                    LossFunction,
                    neurons[last],
                    activations[last],
                    error);
                if (epoch % epochsPoint == 0 && end)
                    m_TrainingSummary.push_back({ epoch, meanError });

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
                m_Layers[0]->backwardPass(prevDelta, prevLayerWeights, neurons[0], input, currDelta);

                batchBegin = batchEnd;
                batchEnd = batchEnd + batchSize < inputs.getHeight() ? batchEnd + batchSize : 0;
            }
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
            m_Layers[i]->forwardPass(activations[i - 1], neurons[i], activations[i]);
        return m_Layers[last]->calculateError(activations[last - 1], outputs, LossFunction, lastNeuron, results, error);
    }
}