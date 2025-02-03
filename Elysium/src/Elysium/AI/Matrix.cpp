#include "Matrix.h"

#include <algorithm>
#include <iterator>

#include "Elysium/Log.h"
#include "Elysium/Random.h"

namespace Elysium
{
    Matrix::Matrix(size_t height, size_t width, bool empty) :
        Width(width),
        Height(height)
    {
        if (!empty) Values.resize(width * height);
    }

    Matrix::Matrix(const std::vector<float>& vector) : Values(vector),
        Width(vector.size()),
        Height(1)
    {
    }

    Matrix::Matrix(const std::vector<std::vector<float>>& vector) : 
        Width(vector[0].size()),
        Height(vector.size())
    {
        size_t size = Height * Width;
        Values.reserve(size);
        for (const std::vector<float>& v : vector)
        {
            if (v.size() > Width)
            {
                Width = 0;
                ELY_CORE_ERROR("Invalid matrix dimensions!");
                return;
            }

            for (float f : v)
                Values.emplace_back(f);
        }
    }

    void Matrix::append(const std::vector<float>& row)
    {
        Width = Width == 0 ? row.size() : Width;
        Values.insert(Values.end(), row.begin(), row.end());
        Height++;
    }

    void Matrix::fill(float value)
    {
        std::fill(Values.begin(), Values.end(), value);
    }

    Matrix Matrix::Concatenate(const Matrix& a, const Matrix& b, int axis)
    {
        Matrix result;
        if (!axis)
        {
            if (a.Width != b.Width)
            {
                ELY_CORE_ERROR("Matrices' width are not equal! ({0} and {1})", a.Width, b.Width);
                return result;
            }

            result = a;
            result.Values.insert(result.Values.end(), b.Values.begin(), b.Values.end());

            result.Width = a.Width;
            result.Height = a.Height + b.Height;
            return result;
        }

        if (a.Height != b.Height)
        {
            ELY_CORE_ERROR("Matrices' height are not equal! ({0} and {1})", a.Height, b.Height);
            return result;
        }

        size_t size = a.Values.size() + b.Values.size();
        result.Values.reserve(size);

        result.Width = a.Width + b.Width;
        result.Height = a.Height;

        for (size_t i = 0; i < result.Height; ++i)
        {
            for (size_t j = 0; j < a.Width; ++j) result.Values.emplace_back(a(i, j));

            for (size_t j = 0; j < b.Width; ++j) result.Values.emplace_back(b(i, j));
        }

        return result;
    }

    Matrix Matrix::Slice(const Matrix& input, size_t r0, size_t r1, size_t c0, size_t c1)
    {
        // Inversion for slicing
        size_t end_row = r1 == 0 ? input.Height : r1;
        size_t end_column = c1 == 0 ? input.Width : c1;

        Matrix result;
        if (r0 > input.getHeight() || end_row > input.getHeight() ||
            c0 > input.getWidth()  || end_column > input.getWidth())
        {
            ELY_CORE_ERROR("Invalid slice indices!");
            return result;
        }

        result.Values.reserve(end_row * end_column);
        for (size_t i = r0; i < end_row; ++i)
        {
            for (size_t j = c0; j < end_column; ++j) result.Values.emplace_back(input(i, j));
        }

        result.Width = end_column - c0;
        result.Height = end_row - r0;
        return result;
    }

    Matrix Matrix::Scramble(const Matrix& input)
    {
        Random::Init();
        std::vector<size_t> indices;
        indices.reserve(input.Height);
        while (indices.size() < input.Height)
        {
            Elysium::Random::InitInteger(0, (int)input.Height - 1);
            int n = Random::Integer();
            if (std::find(indices.begin(), indices.end(), n) == std::end(indices))
                indices.emplace_back((size_t)n);
        }

        Matrix output(input.Height, input.Width);
        for (size_t a = 0; a < input.Height; ++a)
        {
            for (size_t b = 0; b < input.Width; ++b)
                output(a, b) = input(indices[a], b);
        }
        return output;
    }

    Matrix Matrix::row(size_t row)
    {
        Matrix matrix = Matrix(1, Width);

        size_t start = row * Width;
        for (size_t i = 0; i < Width; ++i) matrix.Values[i] = Values[start + i];
        return matrix;
    }

    Matrix Matrix::column(size_t column)
    {
        Matrix matrix = Matrix(1, Height);

        size_t start = column;
        for (size_t i = 0; i < Height; ++i) matrix.Values[i] = Values[start + i * Width];
        return matrix;
    }

    void Matrix::row(size_t row, const std::vector<float>& vector)
    {
        for (size_t i = 0; i < Width; ++i) Values[row * Width + i] = vector[i];
    }

    void Matrix::column(size_t column, const std::vector<float>& vector)
    {
        for (size_t i = 0; i < Height; ++i) Values[i * Width + column] = vector[i];
    }

    void Matrix::print() const
    {
        const size_t maxLength = 64;
        const size_t height = Height <= maxLength ? Height : maxLength;
        const size_t width = Width <= maxLength ? Width : maxLength;
        char matrixStr[2048];
        sprintf(matrixStr, "[");
        for (size_t j = 0; j < height; ++j)
        {
            const char* openBracket = j > 0 ? " [" : "[";
            sprintf(matrixStr, "%s%s", matrixStr, openBracket);

            for (size_t i = 0; i < width; ++i)
            {
                float value = operator()(j, i);
                sprintf(matrixStr, "%s %s%f", matrixStr, value < 0.0f ? "" : " ", value);
            }

            const char* endWidth = Width < maxLength ? "" : " ...";
            const char* closeBracket = j == height - 1 ? " ]" : " ]\n";
            sprintf(matrixStr, "%s%s%s", matrixStr, endWidth, closeBracket);
        }
        const char* endHeight = Height < maxLength ? " ]\n" : "...]\n";
        sprintf(matrixStr, "%s%s", matrixStr, endHeight);
        ELY_INFO("\n{0}", matrixStr);
    }
}