#include "Matrix.h"

#include "Elysium/Log.h"

namespace Elysium
{
    Matrix::Matrix(size_t width, size_t height) : 
        Width(width),
        Height(height)
    {
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
                Width = -1;
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
        size_t iA = 0;
        size_t iB = 0;
        size_t height = 0;
        for (size_t i = 0; i < size; ++i)
        {
            matrixA = iA < a.Width && iB == 0 ? true : false;
            iA = iA < a.Width ? iA : 0;
            iB = iB < b.Width ? iB : 0;
            result.Values.emplace_back(matrixA ? a.Values[iA++ + height * a.Width] : b.Values[iB++ + height * b.Width]);

            if (result.Values.size() % result.Width == 0)
                height++;
        }

        return result;
    }

    Matrix Matrix::Slice(const Matrix& input,
        size_t startColumn, size_t endColumn, size_t startRow, size_t endRow)
    {
        endColumn = endColumn == 0 ? input.Height : endColumn;
        endRow = endRow == 0 ? input.Width : endRow;

        Matrix result;
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

    void Matrix::print()
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
    }
}