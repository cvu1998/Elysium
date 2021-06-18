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
        };

        Model(size_t numberOfLayers);
        ~Model();

        inline void add(HiddenLayer* layer) { m_Layers.emplace_back(layer); }

        inline const std::vector<HiddenLayer*>& getLayers() const { return m_Layers; }
        inline const std::vector<Model::Summary>& getSummary() const { return m_Summary; };

        void fit(const Matrix& inputs, const Matrix& outputs, size_t epochs = 1);

        void predict(const Matrix& inputs, Matrix& results);
        float score(const Matrix& inputs, const Matrix& outputs, Matrix& results);


    public:
        float LearningRate = 0.1f;

        AI::Loss LossFunction = AI::Loss::FORECAST;

    private:
        std::vector<HiddenLayer*> m_Layers;
        std::vector<Model::Summary> m_Summary;
    };
}