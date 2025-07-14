#include "Actions.hpp"
#include "Monster.hpp"

Actions::Actions(int *stats[17], QObject *parent) : QObject{parent},
    m_timers{QTimer(this), QTimer(this), QTimer(this)},
    m_stats{*stats}, m_curAction(WAITING)
{

}

void Actions::startAction(e_action action, int value)
{
    if (m_curAction == WAITING) {
        switch (action) {
        case RESTING:
            startSleeping();
            break;
        case FEEDING:
            startEating(value);
            break;
        case TRAINING:
            startTraining(value);
            break;
        case EXPLORING:
            startExploring(value);
            break;
        case DONJONING:
            startDonjoning(value);
            break;
        default:
            break;
        }
    }
}

Actions::e_action Actions::curAction()
{
    return m_curAction;
}

void Actions::stopCurAction()
{
    m_timers[ACTIONS].stop();
    m_curActionCurTime = 0;
    m_curActionTotalTime = 0;
    m_curAction = WAITING;
    emit actionChanged();
}

void Actions::stopAllActions()
{
    m_timers[STARVING].stop();
    m_timers[HEALING].stop();
    if (m_timers[ACTIONS].isActive())
        m_timers[ACTIONS].stop();
}

int Actions::curTime()
{
    return m_curActionCurTime;
}

int Actions::totalTime()
{
    return m_curActionTotalTime;
}

void Actions::startStarving()
{
    if (*m_stats[Monster::HUNGERCUR] <= 0) {
        if (*m_stats[Monster::HPCUR] > 0) {
            if (m_timers[HEALING].isActive())
                m_timers[HEALING].stop();
            m_stats[Monster::HPCUR] -= 10;
            if (*m_stats[Monster::HPCUR] <= 0) {
                stopAllActions();
                return (emit monsterDied());
            }
        }
        else {
            stopAllActions();
            emit monsterDied();
        }
    }
    else
        *m_stats[Monster::HUNGERCUR] -= 1;
    emit starvingActivated();
}

void Actions::startHealing()
{
    if (*m_stats[Monster::HPCUR] < *m_stats[Monster::HPMAX]) {
        if (m_curAction == RESTING) {
            *m_stats[Monster::HPCUR] += 1 + *m_stats[Monster::LEVEL];
            if (*m_stats[Monster::HPCUR] > *m_stats[Monster::HPMAX])
                *m_stats[Monster::HPCUR] = *m_stats[Monster::HPMAX];
        }
        else
            *m_stats[Monster::HPCUR] += 1;
    }
    else if (m_curAction == RESTING){
        m_curAction = WAITING;
        emit actionChanged();
    }
    if (m_curAction == RESTING)
        m_timers[HEALING].start(1500);
    else
        m_timers[HEALING].start(50000);
    emit healingActivated();
}

void Actions::startSleeping()
{
    m_curAction = RESTING;
    m_timers[HEALING].stop();
    m_timers[HEALING].start(1000);
    emit actionChanged();
}

void Actions::startEating(int food)
{
    if (m_curAction == WAITING) {
        m_curActionTotalTime = m_curActionCurTime = food;
        m_curAction = FEEDING;
        if (m_timers[STARVING].isActive())
            m_timers[STARVING].stop();
        m_timers[ACTIONS].disconnect();
        connect(m_timers, &QTimer::timeout, this, &Actions::eating);
        m_timers[ACTIONS].start(1000);
        emit actionChanged();
        emit actionTimeChanged();
    }
}

void Actions::eating()
{
    if (m_curAction == FEEDING && *m_stats[Monster::FOOD] > 0 &&
            *m_stats[Monster::HUNGERCUR] < *m_stats[Monster::HUNGERMAX]) {
        if (!m_timers[HEALING].isActive())
            m_timers[HEALING].start(10000);
        *m_stats[Monster::HUNGERCUR] += 1;
        *m_stats[Monster::FOOD] -= 1;
        m_curActionCurTime--;
        setEatingTime();
        emit eatingSteps();
    }
}

void Actions::setEatingTime()
{
    if (*m_stats[Monster::HPCUR] < *m_stats[Monster::HPMAX]) {
        *m_stats[Monster::HPCUR] += 1;
    }
    if (m_stats[Monster::FOOD] == 0 || m_curActionCurTime == 0 ||
        m_stats[Monster::HUNGERCUR] >= m_stats[Monster::HUNGERMAX]) {
        if (m_stats[Monster::HUNGERCUR] > m_stats[Monster::HUNGERMAX])
            m_stats[Monster::HUNGERCUR] = m_stats[Monster::HUNGERMAX];
        m_timers[STARVING].start(10000 + getStarvingTolerance());
        m_timers[ACTIONS].stop();
        stopAllActions();
    }
    else
        m_feeder->start(1000);
}

void Actions::startTraining(int time)
{

}

void Actions::training()
{

}

void Actions::setTrainingTime()
{

}

void Actions::startExploring(int level)
{

}

void Actions::exploring()
{

}

void Actions::setExploringTime()
{

}

void Actions::startDonjoning(int level)
{

}

void Actions::donjoning()
{

}

void Actions::setDonjoningTime()
{

}
