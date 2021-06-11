#pragma once

#include <vector>

#include "Elysium/AI/HiddenLayer.h"

namespace Elysium
{
    class Model final
    {
    public:
        Model(size_t numberOfLayers);
        ~Model();

        inline void add(HiddenLayer* layer) { m_Layers.emplace_back(layer); }

        inline const std::vector<HiddenLayer*>& getLayers() const { return m_Layers; }

        void fit(const Matrix& inputs, const Matrix& outputs, size_t epochs = 1);
        void summary();

    private:
        std::vector<HiddenLayer*> m_Layers;
        std::vector<Matrix> m_Neurons;
        std::vector<Matrix> m_Deltas;
        
        Matrix m_Error;
    };
}