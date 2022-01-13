#include "marker.h"

#include <iostream>

Marker::Marker(int l_fileIndex, const std::string& l_filePath, const std::pair<int, int>& l_range)
    : m_fileIndex(l_fileIndex)
    , m_filePath(l_filePath)
    , m_range(l_range)
{}

int Marker::getFileIndex() const
{
    return m_fileIndex;
}

std::string Marker::getFilePath() const
{
    return m_filePath;
}

std::pair<int, int> Marker::getRange() const
{
    return m_range;
}

inline int Marker::getRangeLowerBound() const
{
    return getRange().first;
}

inline int Marker::getRangeUpperBound() const
{
    return getRange().second;
}

std::ostream& operator<<(std::ostream& os, const Marker& marker)
{
    os << "Marker { index = " << marker.m_fileIndex << ", "
       << " range = (" << marker.m_range.first << ", " << marker.m_range.second << ") }";

    return os;
}
