#include "Model.h"

#include "Elysium/Log.h"

namespace Elysium
{
    Model::Model(size_t inputSize) : m_InputSize(inputSize)
    {
    }

    Model::~Model()
    {
        for (HiddenLayer* layer : m_Layers)
            delete layer;
        m_Layers.clear();
    }

    void Model::add(HiddenLayer* layer)
    {
        layer->initWeightAndBiases
        (
            m_Layers.empty() ? m_InputSize : m_Layers.back()->Weights.getHeight()
        );
        m_Layers.push_back(layer);
        if (m_Layers.size() >= 2)
            m_Valid = true;
    }

    void Model::summary() const
    {
        ELY_CORE_INFO("{0}:    {1}", "Layer (Type)", "Param #");
        for (const auto& layer : m_Layers)
            layer->summary();
    }

    void Model::report()
    {
        if (!m_TrainingSummary.empty())
        {
            ELY_CORE_INFO("Training Summary Report");

            size_t n = 0;
            size_t epoch = 1;
            float meanError = 0.0f;
            for (const auto& p : m_TrainingSummary)
            {
                if (epoch != p.Epoch)
                {
                    ELY_CORE_INFO("Epoch: {0}, Error: {1}", epoch, meanError / (float)n);

                    n = 1;
                    epoch = p.Epoch;
                    meanError = p.MeanError;
                    continue;
                }
                meanError += p.MeanError;
                n++;

                if (p == m_TrainingSummary.back())
                {
                    ELY_CORE_INFO("Epoch: {0}, Error: {1}", epoch, meanError / (float)n);
                    return;
                }
            }
        }
        m_TrainingSummary.clear();
    }

    void Model::fit(const Matrix& inputs, const Matrix& outputs, size_t epochs, size_t batchSize)
    {
        if (!m_Valid)
            return;

        const size_t last = m_Layers.size() - 1;
        const size_t epochsPoint = epochs > 20 ? (size_t)((float)epochs * 0.05f) : 1;

        for (size_t epoch = 1; epoch <= epochs; ++epoch)
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

                for (size_t i = 1; i < last; ++i)
                    m_Layers[i]->forwardPass(activations[i - 1], neurons[i], activations[i]);
                float meanError = m_Layers[last]->calculateError(
                    activations[last - 1],
                    output,
                    LossFunction,
                    neurons[last],
                    activations[last],
                    error);
                if (epoch == 1 || epoch % epochsPoint == 0)
                    m_TrainingSummary.push_back({ epoch, meanError });

                Matrix currDelta;
                Matrix prevDelta;
                Matrix prevLayerWeights = m_Layers[last]->Weights;
                Matrix weights;

                m_Layers[last]->LearningRate = LearningRate;
                m_Layers[last]->calculateDelta(error, neurons[last], activations[last - 1], GradientModifier, currDelta);
                prevDelta = currDelta;
                for (int i = (int)last - 1; i > 0; --i)
                {
                    weights = m_Layers[i]->Weights;

                    m_Layers[i]->LearningRate = LearningRate;
                    m_Layers[i]->backwardPass(prevDelta, prevLayerWeights, neurons[i], activations[i - 1], GradientModifier, currDelta);

                    prevDelta = currDelta;
                    prevLayerWeights = weights;
                }
                m_Layers[0]->LearningRate = LearningRate;
                m_Layers[0]->backwardPass(prevDelta, prevLayerWeights, neurons[0], input, GradientModifier, currDelta);

                batchBegin = batchEnd;
                batchEnd = batchEnd + batchSize < inputs.getHeight() ? batchEnd + batchSize : 0;
            }
        }
    }

    void Model::predict(const Matrix& inputs, Matrix& results)
    {
        if (!m_Valid)
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
        if (!m_Valid)
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

    void Model::save(const char* path) const
    {
        _mkdir(path);

        std::string file;
        for (size_t i = 0; i < m_Layers.size(); ++i)
        {
            file = std::string(path) + "/" + std::string(m_Layers[i]->m_LayerName) + std::to_string(i) + ".wb";
            m_Layers[i]->saveWeightsAndBiases(file.c_str());
        }
    }

    bool Model::load(const char* path)
    {
        std::string file;
        for (size_t i = 0; i < m_Layers.size(); ++i)
        {
            file = std::string(path) + "/" + std::string(m_Layers[i]->m_LayerName) + std::to_string(i) + ".wb";
            if (!m_Layers[i]->loadWeightsAndBiases(file.c_str()))
                return false;
        }
        return true;
    }
}