#pragma once

#include <QObject>
#include <QTimer>

class Actions : public QObject
{
     Q_OBJECT

public:
    enum    e_action {
        WAITING,
        RESTING,
        FEEDING,
        TRAINING,
        ONBREAK,
        EXPLORING,
        DONJONING
    };

    explicit Actions(int *stats[17], QObject *parent = nullptr);
    void startAction(e_action action, int value);
    e_action curAction();
    QString curActionStr();
    void stopCurAction();
    void stopAllActions();
    int curTime();
    int totalTime();

private slots:
    void eating();
    void training();
    void exploring();
    void donjoning();

signals:
    e_action actionChanged();
    void actionTimeChanged();
    void starvingActivated();
    void healingActivated();
    void eatingSteps();
    void trainingSteps();
    void exploringSteps();
    void donjoningSteps();
    void monsterDied();

private:
    e_action    m_curAction;
    QTimer      m_timers[3];
    int         *m_stats[17];
    int         m_curActionCurTime;
    int         m_curActionTotalTime;

    enum        e_timers {
        STARVING,
        HEALING,
        ACTIONS
    };

    void startStarving();
    void startHealing();
    void getStarvingTolerance();

    void startSleeping();

    void startEating(int food);
    void setEatingTime();

    void startTraining(int time);
    void setTrainingTime();

    void startExploring(int level);
    void setExploringTime();

    void startDonjoning(int level);
    void setDonjoningTime();
};

