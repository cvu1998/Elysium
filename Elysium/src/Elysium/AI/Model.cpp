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
        if (m_Layers.size() >= 2) m_Valid = true;
    }

    void Model::summary() const
    {
        ELY_CORE_INFO("{0}:    {1}", "Layer (Type)", "Param #");
        for (const auto& layer : m_Layers)
            layer->summary();
    }

    float Model::update(const Matrix& inputs, const Matrix& targets)
    {
        Matrix dL_wrt_dO;
        std::vector<Matrix> neurons(m_Layers.size());
        std::vector<Matrix> activations(m_Layers.size());

        if (!m_Layers[0]->forward(inputs, neurons[0], activations[0]))
        {
            ELY_CORE_ERROR("Invalid input size!");
            return std::numeric_limits<float>::max();
        }

        const size_t last = m_Layers.size() - 1;
        for (size_t i = 1; i < last; ++i)
            m_Layers[i]->forward(activations[i - 1], neurons[i], activations[i]);

        float loss = m_Layers[last]->calculateLoss(
            activations[last - 1],
            targets,
            LossFunction,
            neurons[last],
            activations[last],
            dL_wrt_dO);

        Matrix dL_wrt_dH;
        Matrix dL_wrt_dH_1;
        Matrix weights_1 = m_Layers[last]->Weights;
        Matrix weights;

        m_Layers[last]->LearningRate = LearningRate;
        m_Layers[last]->calculateOutputGradient(dL_wrt_dO, neurons[last], activations[last - 1], dL_wrt_dH);
        dL_wrt_dH_1 = dL_wrt_dH;
        for (int i = (int)last - 1; i > 0; --i)
        {
            weights = m_Layers[i]->Weights;

            m_Layers[i]->LearningRate = LearningRate;
            m_Layers[i]->backward(dL_wrt_dH_1, weights_1, neurons[i], activations[i - 1], dL_wrt_dH);

            dL_wrt_dH_1 = dL_wrt_dH;
            weights_1 = weights;
        }
        m_Layers[0]->LearningRate = LearningRate;
        m_Layers[0]->backward(dL_wrt_dH_1, weights_1, neurons[0], inputs, dL_wrt_dH);

        return loss;
    }

    void Model::fit(const Matrix& inputs, const Matrix& targets, size_t epochs, size_t batchSize)
    {
        if (!m_Valid) return;
;
        const size_t epochsPoint = epochs > 20 ? (size_t)((float)epochs * 0.05f) : 1;

        for (size_t epoch = 1; epoch <= epochs; ++epoch)
        {
            size_t batchBegin = 0;
            size_t batchEnd = batchSize < inputs.getHeight() ? batchSize : 0;
            bool end = false;

            size_t itr = 0;
            float loss = 0.0f;
            while (!end)
            {
                end = batchEnd == 0;
                Matrix input = Matrix::Slice(inputs, batchBegin, batchEnd, 0, 0);
                Matrix target = Matrix::Slice(targets, batchBegin, batchEnd, 0, 0);

                loss += update(input, target);

                batchBegin = batchEnd;
                batchEnd = batchEnd + batchSize < inputs.getHeight() ? batchEnd + batchSize : 0;

                itr++;
            }

            if (Verbose) ELY_INFO("Epoch: {0} Loss: {1}", epoch, loss / (float)itr);
        }
    }

    void Model::predict(const Matrix& inputs, Matrix& predictions)
    {
        if (!m_Valid)
            return;

        const size_t last = m_Layers.size() - 1;

        Matrix lastNeuron;
        std::vector<Matrix> neurons(last);
        std::vector<Matrix> activations(last);

        if (!m_Layers[0]->forward(inputs, neurons[0], activations[0]))
        {
            ELY_CORE_ERROR("Invalid input size!");
            return;
        }

        for (size_t i = 1; i < last; ++i)
            m_Layers[i]->forward(activations[i - 1], neurons[i], activations[i]);
        m_Layers[last]->forward(activations[last - 1], lastNeuron, predictions);
    }

    float Model::score(const Matrix& inputs, const Matrix& targets, Matrix& predictions)
    {
        if (!m_Valid) return 1.0f;

        const size_t last = m_Layers.size() - 1;

        Matrix error;
        Matrix lastNeuron;
        std::vector<Matrix> neurons(last);
        std::vector<Matrix> activations(last);

        if (!m_Layers[0]->forward(inputs, neurons[0], activations[0]))
        {
            ELY_CORE_ERROR("Invalid input size!");
            return std::numeric_limits<float>::max();
        }

        for (size_t i = 1; i < last; ++i)
            m_Layers[i]->forward(activations[i - 1], neurons[i], activations[i]);
        return m_Layers[last]->calculateLoss(activations[last - 1], targets, LossFunction, lastNeuron, predictions, error);
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
            if (!m_Layers[i]->loadWeightsAndBiases(file.c_str())) return false;
        }
        return true;
    }
}