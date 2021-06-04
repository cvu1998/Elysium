#pragma once

#include <vector>

namespace Elysium
{
    struct Coordinate
    {
        size_t column, row;
    };

    class Matrix
    {
    public:
        std::vector<float> Values;

        size_t Height = 0;
        size_t Width = 0;

    public:
        Matrix() = default;
        Matrix(size_t height, size_t width);
        Matrix(const std::vector<float>& vector);
        Matrix(const std::vector<std::vector<float>>& vector);

        static Matrix Concatenate(const Matrix& a, const Matrix& b, bool rowAxis = true);
        static Matrix Slice(const Matrix& input,
            size_t startColumn = 0, size_t endColumn = 0, size_t startRow = 0, size_t endRow = 0);

        inline float& operator[](Coordinate c) { return Values[c.column * Width + c.row]; }
        inline float operator[](Coordinate c) const { return Values[c.column * Width + c.row]; }

        void print();
    };
}