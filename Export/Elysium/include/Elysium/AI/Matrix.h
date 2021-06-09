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

    public:
        Matrix() = default;
        Matrix(size_t height, size_t width, bool empty = false);
        Matrix(const std::vector<float>& vector);
        Matrix(const std::vector<std::vector<float>>& vector);

        inline size_t getHeight() const { return Height; }
        inline size_t getWidth() const { return Width; }

        static Matrix Concatenate(const Matrix& a, const Matrix& b, bool rowAxis = true);
        static Matrix Slice(const Matrix& input,
            size_t startColumn = 0, size_t endColumn = 0, size_t startRow = 0, size_t endRow = 0);

        inline float& operator[](Coordinate c) { return Values[c.column * Width + c.row]; }
        inline float operator[](Coordinate c) const { return Values[c.column * Width + c.row]; }

        void print();

    private:
        size_t Height = 0;
        size_t Width = 0;
    };
}