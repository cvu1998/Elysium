#pragma once

#include <vector>

#include "Elysium/AI/HiddenLayer.h"

namespace Elysium
{
    class Model final
    {
    public:
        struct Summary
        {
            size_t Epoch = 0;
            float MeanError = 0.0f;

            bool operator==(const Summary& s) const { return this->Epoch == s.Epoch && this->MeanError == s.MeanError; }
        };

        Model(size_t inputSize);
        ~Model();

        void add(HiddenLayer* layer);

        inline const std::vector<HiddenLayer*>& getLayers() const { return m_Layers; }

        void summary() const;
        void report();

        void fit(const Matrix& inputs, const Matrix& targets, size_t epochs = 1, size_t batchSize = 1);

        void predict(const Matrix& inputs, Matrix& predictions);
        float score(const Matrix& inputs, const Matrix& targets, Matrix& predictions);

        void save(const char* path) const;
        bool load(const char* path);

    public:
        float LearningRate = 0.1f;

        AI::Loss LossFunction = AI::Loss::MEAN_SQUARED;

    private:
        size_t m_InputSize;
        std::vector<HiddenLayer*> m_Layers;
        std::vector<Model::Summary> m_TrainingSummary;

        bool m_Valid = false;
    };
}