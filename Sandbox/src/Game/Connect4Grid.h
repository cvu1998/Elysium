#pragma once

#include <Elysium.h>

class Connect4Grid
{
private:
    std::array<uint32_t, 42> m_Grid = { 0 };
    std::array<size_t, 7> m_ColumnIndexes = { 0 };

private:
    bool isWinningRow(size_t index, uint32_t value) const;
    bool isWinningColumn(size_t index, uint32_t value) const;
    bool isWinningRightDiagonal(size_t index, uint32_t value) const;
    bool isWinningLeftDiagonal(size_t index, uint32_t value) const;

public:
    Connect4Grid();
    ~Connect4Grid();

    inline bool isValid(size_t column) const { return m_ColumnIndexes[column] < 6; }
    inline uint32_t getValue(int32_t row, int32_t column) const
    {
        if (row >= 0  && row < 6 && column >= 0 && column < 7)
            return m_Grid[(size_t)column * 6 + (size_t)row];
        return 0;
    }

    inline void appendColumn(size_t index, uint32_t value) { m_Grid[index * 6 + m_ColumnIndexes[index]++] = value; }
    inline void clear() 
    {
        m_Grid.fill(0); 
        m_ColumnIndexes.fill(0); 
    }

    bool isWinningMove(size_t index, uint32_t value) const; 
    void getCurrentStateCode(std::string& string);
    void printGrid();
};