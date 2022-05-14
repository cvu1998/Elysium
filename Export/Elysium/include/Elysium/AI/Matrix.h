#pragma once

#include <vector>

namespace Elysium
{
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

        void append(const std::vector<float>& row);
        void fill(float value);

        static Matrix Concatenate(const Matrix& a, const Matrix& b, int axis=0);
        static Matrix Slice(const Matrix& input, size_t r0 = 0, size_t r1 = 0, size_t c0 = 0, size_t c1 = 0);
        static Matrix Scramble(const Matrix& input);

        inline float& operator()(size_t row, size_t column) { return Values[row * Width + column]; }
        inline float operator()(size_t row, size_t column) const { return Values[row * Width + column]; }

        Matrix row(size_t row);
        Matrix column(size_t column);

        void row(size_t row, const std::vector<float>& vector);
        void column(size_t column, const std::vector<float>& vector);

        void print() const;

    private:
        size_t Height = 0;
        size_t Width = 0;
    };
}