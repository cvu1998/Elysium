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
        if (!empty)
            Values.resize(width * height);
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

    Matrix Matrix::Concatenate(const Matrix& a, const Matrix& b, bool rowAxis)
    {
        Matrix result;
        if (rowAxis)
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

        bool matrixA = true;
        for (size_t i = 0; i < result.Height; ++i)
        {
            for (size_t ia = 0; ia < a.Width; ++ia)
                result.Values.emplace_back(a[{i, ia}]);

            for (size_t ib = 0; ib < b.Width; ++ib)
                result.Values.emplace_back(b[{i, ib}]);
        }

        return result;
    }

    Matrix Matrix::Slice(const Matrix& input,
        size_t startColumn, size_t endColumn, size_t startRow, size_t endRow)
    {
        endColumn = endColumn == 0 ? input.Height : endColumn;
        endRow = endRow == 0 ? input.Width : endRow;

        Matrix result;
        if (startColumn > input.getHeight() || 
            endColumn > input.getHeight() ||
            startRow > input.getWidth() || 
            endRow > input.getWidth())
        {
            ELY_CORE_ERROR("Invalid slice indices!");
            return result;
        }

        result.Values.reserve(endColumn * endRow);
        for (size_t j = startColumn; j < endColumn; ++j)
        {
            for (size_t i = startRow; i < endRow; ++i)
                result.Values.emplace_back(input[{j, i}]);
        }

        result.Width = endRow - startRow;
        result.Height = endColumn - startColumn;
        return result;
    }

    Matrix Matrix::Scramble(const Matrix& input)
    {
        Random::Init();
        std::vector<size_t> indices;
        indices.reserve(input.Height);
        while (indices.size() < input.Height)
        {
            int n = Random::Integer(0, (int)input.Height - 1);
            if (std::find(indices.begin(), indices.end(), n) == std::end(indices))
                indices.emplace_back((size_t)n);
        }

        Matrix output(input.Height, input.Width);
        for (size_t a = 0; a < input.Height; ++a)
        {
            for (size_t b = 0; b < input.Width; ++b)
                output[{a, b}] = input[{indices[a], b}];
        }
        return output;
    }

    void Matrix::print() const
    {
        printf("[");
        for (size_t j = 0; j < Height; ++j)
        {
            const char* openBracket = j > 0 ? " [" : "[";
            printf(openBracket);

            for (size_t i = 0; i < Width; ++i)
                printf(" %f", operator[]({j, i}));

            const char* closeBracket = j == Height - 1 ? "]" : "]\n";
            printf(closeBracket);
        }
        printf("]\n");
        fflush(stdout);
    }
}