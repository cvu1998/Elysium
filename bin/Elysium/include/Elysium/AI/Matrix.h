#pragma once

#include <vector>

namespace Elysium
{
    class Matrix
    {
    public:
        std::vector<float> Values;

        size_t Width = 0;
        size_t Height = 0;

    public:
        Matrix() = default;
        Matrix(const std::vector<float>& vector);
        Matrix(const std::vector<std::vector<float>>& vector);

        static Matrix Concatenate(const Matrix& a, const Matrix& b, bool rowAxis = true);
        static Matrix Slice(const Matrix& input,
            size_t startColumn = 0, size_t endColumn = 0, size_t startRow = 0, size_t endRow = 0);

        float operator()(size_t x, size_t y) const;

        void print();
    };
}