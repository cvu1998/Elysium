#pragma once

#include <vector>

#include "Elysium/Math.h"
#include "Elysium/AI/HiddenLayer.h"

namespace Elysium
{
    class Model final
    {
    public:
        Model(size_t inputSize);
        ~Model();

        void add(HiddenLayer* layer);

        inline const std::vector<HiddenLayer*>& getLayers() const { return m_Layers; }

        void summary() const;

        float update(const Matrix& inputs, const Matrix& targets);
        void fit(const Matrix& inputs, const Matrix& targets, size_t epochs = 1, size_t batchSize = 1);

        void predict(const Matrix& inputs, Matrix& predictions);
        float score(const Matrix& inputs, const Matrix& targets, Matrix& predictions);

        void clipGradients(float value = 1.0f) { m_ClipValue = glm::abs(value); }

        void save(const char* path) const;
        bool load(const char* path);

    public:
        bool Verbose = true;
        float LearningRate = 0.1f;
        float Regurlarization = 0.01f;

        AI::Loss LossFunction = AI::Loss::MEAN_SQUARED;

    private:
        size_t m_InputSize;
        std::vector<HiddenLayer*> m_Layers;

        bool m_Valid = false;

        float m_ClipValue = 0.0f;
    };
}