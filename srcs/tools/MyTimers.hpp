#ifndef MYTIMERS_HPP
#define MYTIMERS_HPP

#include <QObject>
#include <QTimer>

class MyTimers : public QObject
{
    Q_OBJECT
public:
    explicit MyTimers(QObject *parent = nullptr);

    void startTraining();

signals:

private:
    enum e_timers {
        STARVER,
        HEALER,
        ACTIONER
    };
    QTimer  m_timers[6];
    int     m_timeActionCur;
    int     m_timeAction;

};

#endif // MYTIMERS_HPP
