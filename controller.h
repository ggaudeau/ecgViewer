#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QLineSeries>
#include <QValueAxis>
#include <QtQml/qqml.h>

#include "sequence.h"

/**
 * @brief The Controller class
 *
 * We separate program into two blocks:
 *  - logic and model (C++)
 *  - view (QML)
 *
 *  It makes glu between QML and C++.
 *  There is no callback from C++ to QML.
 *  All calls are triggered by the end user.
 */

class Controller : public QObject
{
    Q_OBJECT

    Q_PROPERTY(quint32 windowWidthInSeconds
               READ getWindowWidthInSeconds WRITE setWindowWidthInSeconds
               NOTIFY windowWidthInSecondsChanged)

    QML_ELEMENT

    quint64 m_startTimeMs;
    quint64 m_pauseTimeMs;
    quint8 m_windowWidthInSeconds;
    Sequence m_sequence;

public:
    Controller(QObject* parent = nullptr);
    ~Controller();

    quint32 getWindowWidthInSeconds() const;
    void setWindowWidthInSeconds(quint32 l_windowWidthInSeconds);

    Q_INVOKABLE bool loadSequence(const QString& directory);
    Q_INVOKABLE void applyCorrection(int averageWindowWidth);
    Q_INVOKABLE void resetCorrection();
    Q_INVOKABLE void removeOutliers();

    Q_INVOKABLE void play();
    Q_INVOKABLE void resume();
    Q_INVOKABLE void pause();
    Q_INVOKABLE void clear();

    Q_INVOKABLE bool init(QLineSeries* lineSeries, QValueAxis* xValueAxis, QValueAxis* yValueAxis);
    Q_INVOKABLE bool update(QLineSeries* lineSeries, QValueAxis* xValueAxis, QValueAxis* yValueAxis);
    Q_INVOKABLE qreal getProgressValue() const;

private:
    void globFit(QValueAxis* xValueAxis, QValueAxis* yValueAxis);
    bool _update(QLineSeries* lineSeries, QValueAxis* xValueAxis, QValueAxis* yValueAxis, size_t offset);

signals:
    void windowWidthInSecondsChanged();
};

#endif // CONTROLLER_H
