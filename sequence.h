#ifndef SEQUENCE_H
#define SEQUENCE_H

#include <vector>
#include <list>
#include <string>
#include <map>

#include "marker.h"

class Marker;

/**
 * @brief The Sequence class
 *
 * Model class that contains data from files.
 * In fact, this class handles itself logic about file reading
 * and sequence corrections.
 */

class Sequence {

    std::vector<int16_t> m_values;
    std::vector<int16_t> m_correctedValues;
    std::pair<int16_t, int16_t> m_minMaxValues;
    uint32_t m_frequencyHz;
    std::list<Marker> m_markers; // just in case

public:
    Sequence();

    bool readSequenceFromFiles(const std::list<std::string>& filePaths);
    void applyCorrection(int averageWindowWidth);
    void resetCorrection();
    void removeOutliers();

    void reset();

    const std::vector<int16_t>& getValues() const;
    std::pair<int16_t, int16_t> getMinMaxValues() const;
    uint32_t getFrequencyHz() const;
    const std::list<Marker>& getMarkers() const;

    friend std::ostream& operator<<(std::ostream& os, const Sequence& seq);    

private:
    void readSequenceFromFile(const std::string& filePath, int& sampleSize);
    int16_t searchMaxValue() const;
    int16_t searchMinValue() const;
};

#endif // SEQUENCE_H
