#pragma once

#include <QObject>
#include <QRandomGenerator>
#include <QTimer>
#include <QString>
#include <QSettings>
#include <MyLabel.hpp>

#include "Equipement.hpp"

class Monster : public QObject
{
    Q_OBJECT

public:
    enum    e_type {
        FIRE,
        WATER,
        EARTH,
        AIR,
        DARK,
        LIGHT,
        ARCANE,
        CHAOS
    };

    enum    e_action {
        WAITING,
        RESTING,
        FEEDING,
        TRAINING,
        ONBREAK,
        EXPLORING,
        DONJONING
    };

    enum    e_stats {
        LEVEL,
        XPCUR,
        XPMAX,
        HPCUR,
        HPMAX,
        HUNGERCUR,
        HUNGERMAX,
        TOPLACE,
        STR,
        STRBOOST,
        END,
        ENDBOOST,
        SPD,
        SPDBOOST,
        FOOD,
        GOLD,
        LVLACTIONCUR,
        LVLEXPLOMAX
    };

    Monster(const QString name);
    Monster(const QString name, const e_type type, const int stats[13]);
    ~Monster();

    QString getName();
    e_type getType();
    int getTypeInt();
    QString getTypeStr();
    int getStats(const e_stats stats);
    e_action getAction();
    QString getActionStr();
    int getTime();
    int getCurTime();
    bool isAlive();
    void clearEquipement();
    std::shared_ptr<Equipement> getEquipement(const bool equiped, const int pos) const;

    void update();
    void feed(int food);        // Action
    void train(int time);       // Action
    void explore(int level);    // Action
    void donjon(int level);        // Action
    void addEquipement(const std::shared_ptr<Equipement> item, const bool equiped, const int pos);
    void setItemToSell(const int itemNb);
    void sellItem();

public slots:
    void rest();                // Action
    void spendPointOnStr();
    void spendPointOnEnd();
    void spendPointonSpd();
    void setEquipement(const MyLabel *clicked);
    void unsetEquipement(const MyLabel *clicked);

private slots:
    void starving();
    void healing();
    void feeding();
    void training();
    void exploring();
    void donjoning();

signals:
    void levelChanged();
    void xpChanged();
    void hpChanged();
    void hungerChanged();
    void foodGained();
    void goldGained();
    void actionChanged();
    void actionTimeChanged();
    void aliveChanged();
    void statsToPlaceChanged();
    void statsStrChanged();
    void statsEndChanged();
    void statsSpdChanged();
    void equipementChanged();

protected:
    QString     m_name;
    e_type      m_type;
    bool        m_alive;
    int         m_stats[18];
    e_action    m_action;
    int         m_time;
    int         m_curTime;
    int         m_itemToSell;
    QTimer      m_starver;
    QTimer      m_healer;
    QTimer      m_feeder;
    QTimer      m_trainer;
    QTimer      m_explorer;
    QTimer      m_donjoner;
    QList<std::shared_ptr<Equipement>>  m_wearing;
    QList<std::shared_ptr<Equipement>>  m_inventory;

private:
    void setNewMonster(QString name);
    void setMonster();
    void setInventory();
    void levelUp();
    int getExploringTotalTime(int level);
    int getStarvingTolerance();
    int getInSpeedEfficiency();
    int getOutSpeedEfficiency(int total);
    int getStrengthEfficiency(int divisor);
    void setStarvingTime();
    void setEatingTime();
    void setTrainingTime();
    void setBreakTraining();
    void setExploringTime();
    void setDonjoningTime();
    void stopAction();
    void stopAllActions();
    void addItemToInventory(std::shared_ptr<Equipement> eq);
    void removeItemFromInventory(std::shared_ptr<Equipement> eq);
    void gainDonjon();
    int calculHP();
    int calculHunger();
    int calculDonjonGold();
    void setDeath();
};
