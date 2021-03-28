#pragma once

#include <functional>

namespace Elysium
{
    namespace ImGuiUtility
    {
        void drawImGuiLabelWithColumn(const char* label, std::function<void(void)> function, float columnWidth = 100.0f);

        void createImGuiDockspace(std::function<void(void)> function);
    }
}