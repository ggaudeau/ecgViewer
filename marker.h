#ifndef MARKER_H
#define MARKER_H

#include <string>
#include <map>

/**
 * @brief The Marker class
 *
 * Sub-model class that keeps information about file parsed
 */

class Marker
{
    int const m_fileIndex;
    std::string const m_filePath;
    std::pair<int, int> m_range;

public:
    Marker(int l_fileIndex, const std::string& l_filePath, const std::pair<int, int>& l_range);

    int getFileIndex() const;
    std::string getFilePath() const;
    std::pair<int, int> getRange() const;
    inline int getRangeLowerBound() const;
    inline int getRangeUpperBound() const;

    friend std::ostream& operator<<(std::ostream& os, const Marker& marker);
};

#endif // MARKER_H
