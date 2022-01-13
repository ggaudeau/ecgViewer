#include "sequence.h"

#include <fstream>
#include <cassert>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <numeric>

#include <boost/histogram.hpp>
#include <boost/format.hpp>

Sequence::Sequence()
    : m_minMaxValues(0,0)
    , m_frequencyHz(500)
{
}

void Sequence::readSequenceFromFile(const std::string& filePath, int& sampleSize) {
    std::ifstream ifs(filePath);
    sampleSize = -1;

    if (ifs) {
        std::string line;

        // header: "NbData=108"
        if (std::getline(ifs, line)) {
            size_t pos = line.find("NbData=");
            if (pos != std::string::npos) {
                if (line.size() > 7) {
                    std::string const str = line.substr(7, line.size() - 7);

                    std::istringstream iss(str);
                    iss >> sampleSize;
                }
            }

            // data: int16_t
            int checkCounter = 0;
            if (sampleSize > 0) {
                assert(sampleSize < 100000);
                m_values.reserve(m_values.size() + sampleSize);

                do {
                    std::getline(ifs, line);
                    if (!line.empty()) {
                        int16_t value = 0;
                        std::istringstream iss(line);
                        iss >> value;
                        m_values.push_back(value);
                        ++checkCounter;
                    }
                } while (ifs);

                assert(sampleSize == checkCounter);
            }
        }
        ifs.close();
    }
}

void Sequence::applyCorrection(int averageWindowWidth)
{
    assert(averageWindowWidth>0);
    assert(averageWindowWidth<10000);

    int* matrix = new int[averageWindowWidth];

    m_correctedValues.clear();

    m_correctedValues.reserve(m_values.size());
    for (int i = 0 ; static_cast<size_t>(i) < m_values.size() ; ++i) {

        int indexMin = std::max(0, i - averageWindowWidth / 2);
        int indexMax = std::min(static_cast<int>(m_values.size() - 1), i + averageWindowWidth / 2);

        auto begin = m_values.begin() + indexMin;
        auto end = m_values.begin() + indexMax;

        m_correctedValues.push_back( std::accumulate(begin, end, 0) / (indexMax - indexMin) );
    }

    m_minMaxValues.first = searchMinValue();
    m_minMaxValues.second = searchMaxValue();

    delete [] matrix;
}

void Sequence::resetCorrection()
{
    m_correctedValues.clear();

    m_minMaxValues.first = searchMinValue();
    m_minMaxValues.second = searchMaxValue();
}

void Sequence::removeOutliers()
{
    auto histogram = boost::histogram::make_histogram( boost::histogram::axis::regular<>(10, m_minMaxValues.first, m_minMaxValues.second, "Y-values") );

    std::vector<int16_t>& values = (m_correctedValues.empty()) ? m_values : m_correctedValues;

    std::for_each( values.cbegin(), values.cend(), std::ref(histogram) );

    std::ostringstream os;
    for (auto x : indexed(histogram, boost::histogram::coverage::all)) {
        os << boost::format("bin %2i [%4.1f, %4.1f): %i\n") % x.index() % x.bin().lower() %
              x.bin().upper() % *x;
    }
    std::cout << "DEBUG: " << os.str() << std::flush;

    int16_t minY=std::numeric_limits<int16_t>::max(), maxY=std::numeric_limits<int16_t>::min();

    for (auto x : indexed(histogram, boost::histogram::coverage::all)) {
        if (*x > values.size() / 200) {
            std::cout << *x << std::endl;
            if (minY > x.bin().lower()) {
                minY = x.bin().lower();
            }
            if (maxY < x.bin().upper()) {
                maxY = x.bin().upper();
            }
        }
    }

    std::cout << "DEBUG: keep only values in [" << minY << ", " << maxY << "]" << std::endl;

    // Beware: this action is not reversible in program ! (require to restart program)

    auto it = values.begin();
    while (it != values.end()) {
        if (*it > maxY) {
            *it = maxY;
        }
        if (*it < minY) {
            *it = minY;
        }
        ++it;
    }

    m_minMaxValues.first = searchMinValue();
    m_minMaxValues.second = searchMaxValue();
}

void Sequence::reset()
{
    m_values.clear();
    m_correctedValues.clear();
    m_markers.clear();
}

const std::vector<int16_t>& Sequence::getValues() const
{
    return (m_correctedValues.empty()) ? m_values : m_correctedValues;
}

std::pair<int16_t, int16_t> Sequence::getMinMaxValues() const
{
    return m_minMaxValues;
}

uint32_t Sequence::getFrequencyHz() const
{
    return m_frequencyHz;
}

const std::list<Marker>& Sequence::getMarkers() const
{
    return m_markers;
}

std::ostream& operator<<(std::ostream& os, const Sequence& seq)
{
    const std::vector<int16_t>& values = (seq.m_correctedValues.empty() ? seq.m_values : seq.m_correctedValues);

    os << "Sequence { frequency = " << seq.m_frequencyHz << " Hz," << std::endl
       << "\tvalues = { ... } size = " << values.size() << ", " << std::endl
       << "\tmarkers = ";

    std::list<Marker>::const_iterator cit = seq.m_markers.cbegin();
    while (cit != seq.m_markers.cend()) {
        os << "\t\t" << *cit << std::endl;
        ++cit;
    }
    os << "}";
    return os;
}

bool Sequence::readSequenceFromFiles(const std::list<std::string>& filePaths) {
    std::list<std::string>::const_iterator cit = filePaths.cbegin();
    int i = 0;
    int rangeIndex = 0;

    while (cit != filePaths.cend()) {
        const std::string& filePath = *cit;
        int sampleSize;

        readSequenceFromFile(filePath, sampleSize);

        m_minMaxValues.first = searchMinValue();
        m_minMaxValues.second = searchMaxValue();

        if (sampleSize > 0) {
            std::pair<int, int> range;

            range.first = rangeIndex;
            rangeIndex += sampleSize;
            range.second = rangeIndex - 1;

            m_markers.push_back( Marker(i, filePath, range) );

        } else {
            std::cout << "ERROR: interrupt parsing" << std::endl;
            return false;
        }

        ++i;
        ++cit;
    }
    return true;
}

int16_t Sequence::searchMaxValue() const
{
    int maxValue = -1;

    const std::vector<int16_t>& values = (m_correctedValues.empty() ? m_values : m_correctedValues);

    auto it = std::max_element( std::begin(values), std::end(values) );

    if (it != values.cend()) {
        maxValue = *it;
    }

    return maxValue;
}

int16_t Sequence::searchMinValue() const
{
    int maxValue = -1;

    const std::vector<int16_t>& values = (m_correctedValues.empty() ? m_values : m_correctedValues);

    auto it = std::min_element( std::begin(values), std::end(values) );

    if (it != values.cend()) {
        maxValue = *it;
    }

    return maxValue;
}
