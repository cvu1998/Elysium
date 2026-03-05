#pragma once

#include <memory>
#include <vector>

#include "Elysium/Layer.h"

namespace Elysium
{
    class LayerStack
    {
    private:
        std::vector <std::unique_ptr<Layer>> m_Layers;
        std::vector<std::unique_ptr<Layer>>::iterator m_LayerInsert;

    public:
        LayerStack();
        ~LayerStack() { }

        template<typename T, typename ... Args>
        void pushLayer(Args&& ... args)
        {
            m_LayerInsert = m_Layers.emplace(m_LayerInsert, std::make_unique<T>(std::forward<Args>(args) ...));
        }

        template<typename T, typename ... Args>
        void pushOverlay(Args&& ... args)
        {
            m_Layers.emplace_back(std::make_unique<T>(std::forward<Args>(args) ...));
        }

        template<typename T>
        void popLayer()
        {
            for (size_t i = 0; i < m_Layers.size(); ++i)
            {
                if (dynamic_cast<T*>(m_Layers[i].get()))
                {
                    m_Layers.erase(m_Layers.begin() + i);
                    m_LayerInsert--;
                    break;
                }
            }
        }

        template<typename T>
        void popOverlay()
        {
            for (size_t i = 0; i < m_Layers.size(); ++i)
            {
                if (dynamic_cast<T*>(m_Layers[i].get()))
                {
                    m_Layers.erase(m_Layers.begin() + i);
                    break;
                }
            }
        }

        template<typename T>
        bool containsLayer()
        {
            for (auto& layer : m_Layers)
            {
                if (dynamic_cast<T*>(layer.get()))
                {
                    return true;
                }
            }
            return false;
        }

        void inline clearLayerStack() { m_Layers.clear(); }

        std::vector<std::unique_ptr<Layer>>::iterator begin() { return m_Layers.begin(); }
        std::vector<std::unique_ptr<Layer>>::iterator end() { return m_Layers.end(); }
    };
}

