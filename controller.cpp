#include "controller.h"
#include "tool.h"

#include <QDateTime>
#include <QDebug>

Controller::Controller(QObject* parent)
    : QObject(parent)
    , m_startTimeMs(0)
    , m_pauseTimeMs(0)
    , m_windowWidthInSeconds(6) {
}

Controller::~Controller() {
}

quint32 Controller::getWindowWidthInSeconds() const
{
    return m_windowWidthInSeconds;
}

void Controller::setWindowWidthInSeconds(quint32 l_windowWidthInSeconds)
{
    m_windowWidthInSeconds = l_windowWidthInSeconds;
}

void Controller::play()
{
    m_startTimeMs = QDateTime::currentMSecsSinceEpoch();
}

void Controller::resume()
{
    quint64 currentTimeMs = QDateTime::currentMSecsSinceEpoch();
    quint64 deltaTimeMs = m_pauseTimeMs - m_startTimeMs;

    m_startTimeMs = currentTimeMs - deltaTimeMs;
}

void Controller::pause()
{
    m_pauseTimeMs = QDateTime::currentMSecsSinceEpoch();
}

void Controller::clear()
{
    m_sequence.reset();
}

bool Controller::loadSequence(const QString& directory)
{
    std::list<std::string> paths;

    if (Tool::listECGFilesInDirectory(directory.toLocal8Bit().data(), paths)) {
        m_sequence.reset();

        if (m_sequence.readSequenceFromFiles(paths)) {
            qDebug() << "DEBUG: sequence in directory '" << directory << "' successfully loaded";
            return true;
        }
    }
    return false;
}

void Controller::applyCorrection(int averageWindowWidth)
{
    m_sequence.applyCorrection(averageWindowWidth);
}

void Controller::resetCorrection()
{
    m_sequence.resetCorrection();
}

void Controller::removeOutliers()
{
    m_sequence.removeOutliers();
}

static quint32 calculateSampleOffset(quint32 frequencyHz, quint64 deltaTimeMs) {
    assert(frequencyHz > 0);

    return (frequencyHz * static_cast<qreal>(deltaTimeMs / 1000.0));
}

bool Controller::init(QLineSeries* lineSeries, QValueAxis* xValueAxis, QValueAxis* yValueAxis)
{
    bool res = _update(lineSeries, xValueAxis, yValueAxis, 0);

    globFit(xValueAxis, yValueAxis);

    return res;
}

bool Controller::update(QLineSeries* lineSeries, QValueAxis* xValueAxis, QValueAxis* yValueAxis)
{
    quint64 deltaTimeMs = QDateTime::currentMSecsSinceEpoch() - m_startTimeMs;

    size_t offset = static_cast<size_t>(calculateSampleOffset( m_sequence.getFrequencyHz(), deltaTimeMs ));

    return _update(lineSeries, xValueAxis, yValueAxis, offset);
}

bool Controller::_update(QLineSeries* lineSeries, QValueAxis* xValueAxis, QValueAxis* yValueAxis, size_t offset)
{
    bool endOfSampling = false;

    const std::vector<int16_t>& samples = m_sequence.getValues();

    // Calculate number of samples displayed on screen
    //  it depends on frequency and window width

    if (offset < samples.size()) {
        size_t numberOfSampleInWindow = m_sequence.getFrequencyHz() * m_windowWidthInSeconds;
        if (offset + numberOfSampleInWindow > samples.size()) {
            numberOfSampleInWindow = samples.size() - offset;            

            endOfSampling = true;
        }        

        // Calculate new points to display
        //  it depends on elapsed time since start time

        QList<QPointF> points;

        points.reserve(numberOfSampleInWindow);
        for (size_t i = 0 ; i < numberOfSampleInWindow ; ++i) {
            size_t index = offset + i;
            qreal x = index / static_cast<qreal>( m_sequence.getFrequencyHz() );

            points.push_back( QPointF(x, samples[index]) );
        }

        // Update X-axis

        if (xValueAxis->min() != offset) {
            xValueAxis->setMin(offset / static_cast<qreal>(m_sequence.getFrequencyHz()));
        }
        if (xValueAxis->max() != offset + numberOfSampleInWindow) {
            xValueAxis->setMax((offset + numberOfSampleInWindow) / static_cast<qreal>(m_sequence.getFrequencyHz()));
        }

        // Ignore Y-axis
        //  indeed we choose to update it once globally in order to have a smooth payback

        (void) yValueAxis;

        // Update curves
        //  at the end we update the graphic model itself

        lineSeries->replace(points);

    } else {
        qDebug() << "WARN: end of sampling data reached (" << offset << " > " << samples.size() << ")";
    }

    if (endOfSampling) {
        return false; // insidious signal sent to the view to force stopping timer
    }
    return true;
}

qreal Controller::getProgressValue() const
{
    quint64 deltaTimeMs = QDateTime::currentMSecsSinceEpoch() - m_startTimeMs;

    size_t offset = static_cast<size_t>(calculateSampleOffset( m_sequence.getFrequencyHz(), deltaTimeMs ));

    const std::vector<int16_t>& samples = m_sequence.getValues();

    size_t numberOfSampleInWindow = m_sequence.getFrequencyHz() * m_windowWidthInSeconds;

    return ( offset / static_cast<qreal>(samples.size() - numberOfSampleInWindow) );
}

void Controller::globFit(QValueAxis* xValueAxis, QValueAxis* yValueAxis)
{
    std::pair<int16_t, int16_t> minMaxValues = m_sequence.getMinMaxValues();

    int const minYValue = minMaxValues.first;
    int const maxYValue = minMaxValues.second;    

    (void) xValueAxis;

    // update Y-axis

    if (yValueAxis->min() != minYValue) {
        yValueAxis->setMin(minYValue);
    }
    if (yValueAxis->max() != maxYValue) {
        yValueAxis->setMax(maxYValue);
    }
}
