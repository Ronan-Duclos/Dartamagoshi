#include "Monster.hpp"

#include <QtMath>
//#include <QSettings>
#include <QDebug>

#include "EquipementFabricator.hpp"

Monster::Monster(const QString name)
{
    setInventory();
    setNewMonster(name);
}

Monster::Monster(const QString name, const e_type type, const int stats[]) :
    m_name(name), m_type(type), m_alive(true),
    m_stats{stats[0], stats[1], stats[2], stats[3], stats[4], stats[5],
            stats[6], stats[7], stats[8], 0, stats[9], 0, stats[10], 0,
            stats[11], stats[12], 0, stats[13]},
    m_action(WAITING), m_starver(this), m_healer(this),
    m_feeder(this), m_trainer(this), m_explorer(this), m_donjoner(this)
{
    setInventory();
    setMonster();
}

Monster::~Monster()
{
    clearEquipement();
}

void Monster::setNewMonster(QString name)
{
    m_name = name;
    m_type = static_cast<e_type>(QRandomGenerator::global()->generate() % 8);
    m_stats[LEVEL] = 1;
    m_stats[XPCUR] = 0;
    m_stats[XPMAX] = 10;
    m_stats[HPCUR] = 100;
    m_stats[HPMAX] = 100;
    m_stats[HUNGERCUR] = 50;
    m_stats[HUNGERMAX] = 50;
    m_stats[TOPLACE] = 1;
    m_stats[STR] = 1;
    m_stats[STRBOOST] = 0;
    m_stats[END] = 1;
    m_stats[ENDBOOST] = 0;
    m_stats[SPD] = 1;
    m_stats[SPDBOOST] = 0;
    m_stats[FOOD] = 100;
    m_stats[GOLD] = 0;
    m_stats[LVLACTIONCUR] = 0;
    m_stats[LVLEXPLOMAX] = 1;
    m_alive = true;
    setMonster();
}

void Monster::setMonster()
{
    connect(&m_starver, &QTimer::timeout, this, &Monster::starving);
    connect(&m_healer, &QTimer::timeout, this, &Monster::healing);
    connect(&m_feeder, &QTimer::timeout, this, &Monster::feeding);
    connect(&m_trainer, &QTimer::timeout, this, &Monster::training);
    connect(&m_explorer, &QTimer::timeout, this, &Monster::exploring);
    connect(&m_donjoner, &QTimer::timeout, this, &Monster::donjoning);
    m_starver.start(10000 + 1000 * (m_stats[LEVEL] - 1));               // to move in start()
    m_healer.start(10000);                                              // to move in start()
    m_action = WAITING;                                                 // to move in start()
}

void Monster::setInventory()
{
    std::shared_ptr<Equipement> tmp(nullptr);

    m_wearing.resize(4);
    for (int i = 0; i < 4; i++)
        m_wearing.push_back(tmp);
    for (int i = 0; i < 16; i++)
        m_inventory.push_back(tmp);
}

int Monster::calculHP()
{
    return 100 + 10 * m_stats[LEVEL] + 20 * (m_stats[STR] + m_stats[STRBOOST]);
}

int Monster::calculHunger()
{
    return 50 + 10 * m_stats[LEVEL] + 20 * (m_stats[END] + m_stats[ENDBOOST]);
}

int Monster::calculDonjonGold()
{
    return 10 * m_stats[LVLACTIONCUR] + (QRandomGenerator::global()->generate() % (10 * m_stats[LVLACTIONCUR]));
}

void Monster::levelUp()
{
    m_stats[XPCUR] = 0;
    m_stats[HPCUR] = m_stats[HPMAX] = calculHP();
    m_stats[HUNGERCUR] = m_stats[HUNGERMAX] = calculHunger();
    if (m_stats[LEVEL] < 100)
        m_stats[LEVEL]++;
    m_stats[XPMAX] = 10 * qPow(m_stats[LEVEL], 1.86);
    m_stats[TOPLACE] += 2;
    emit hungerChanged();
    update();
}

int Monster::getExploringTotalTime(int level)
{
    return 60 + 15 * (level - 1);
}

int Monster::getStarvingTolerance()
{
    return 1000 + (m_stats[LEVEL] * 100) + (((m_stats[END] + m_stats[ENDBOOST]) / 1.5) * 200);
}

int Monster::getInSpeedEfficiency()
{
    return m_stats[LEVEL] * 100 + ((m_stats[SPD] + m_stats[SPDBOOST]) / 1.5) * 200;
}

int Monster::getOutSpeedEfficiency(int total)
{
    return total * (m_stats[LEVEL] + (m_stats[SPD] + m_stats[SPDBOOST]) / 1.5) / 100;
}

int Monster::getStrengthEfficiency(int divisor)
{
    return ((m_stats[STR] + m_stats[STRBOOST]) / 1.5) / divisor;
}

void Monster::setStarvingTime()
{
    int time;

    if (m_action == RESTING)
        time = 20000 + getStarvingTolerance();
    else if (m_action == TRAINING || m_action == EXPLORING)
        time = 5000 + getStarvingTolerance();
    else
        time = 10000 + getStarvingTolerance();
    m_starver.start(time);
}

void Monster::setEatingTime()
{
    if (m_stats[HPCUR] < m_stats[HPMAX]) {
        m_stats[HPCUR]++;
        emit hpChanged();
    }
    if (m_stats[FOOD] == 0 || m_stats[HUNGERCUR] >= m_stats[HUNGERMAX] || m_curTime == 0) {
        if (m_stats[HUNGERCUR] > m_stats[HUNGERMAX])
            m_stats[HUNGERCUR] = m_stats[HUNGERMAX];
        m_starver.start(10000 + getStarvingTolerance());
        m_feeder.stop();
        stopAction();
    }
    else
        m_feeder.start(1000);
}

void Monster::setTrainingTime()
{
    if (m_action == TRAINING) {
        if (m_curTime > 20) {
            int tmp = 20000 - getInSpeedEfficiency();
            m_trainer.start(tmp);
            m_curTime -= (tmp / 1000);
        }
        else if (m_curTime <= 0) {
            m_healer.start(10000);
            m_trainer.stop();
            stopAction();
        }
        else {
            m_trainer.start(m_curTime * 1000);
            m_curTime = 0;
        }
    }
    else {
        if (m_curTime > 1) {
            m_trainer.start(1000);
            m_curTime -= 1;
        }
        else {
            m_healer.start(10000);
            m_trainer.stop();
            stopAction();
        }
    }
}

void Monster::setBreakTraining()
{
    m_stats[HUNGERCUR]++;
    m_stats[FOOD]--;
    m_curTime -= 1;
    setTrainingTime();
    emit hungerChanged();
    emit actionTimeChanged();
    if (m_stats[HUNGERCUR] == m_stats[HUNGERMAX]) {
        m_action = TRAINING;
        m_starver.start(5000 + getStarvingTolerance());
        emit actionChanged();
    }
}

void Monster::setExploringTime()
{
    if (m_curTime > 10) {
        int tmp = 10000 - getInSpeedEfficiency();
        m_explorer.start(tmp);
        m_curTime -= (tmp / 1000);
    }
    else if (m_curTime <= 0) {
        m_healer.start(10000);
        m_explorer.stop();
        stopAction();
        emit foodGained();
    }
    else {
        m_explorer.start(m_curTime * 1000);
        m_curTime = 0;
    }
}

void Monster::setDonjoningTime()
{
    if (m_curTime > 10) {
        int tmp = 10000 - getInSpeedEfficiency();
        m_donjoner.start(tmp);
        m_curTime -= (tmp / 1000);
    }
    else if (m_curTime <= 0) {
        m_healer.start(10000);
        m_donjoner.stop();
        stopAction();
        gainDonjon();
    }
    else {
        m_donjoner.start(m_curTime * 1000);
        m_curTime = 0;
    }
}

void Monster::stopAction()
{
    m_curTime = 0;
    m_time = 0;
    m_action = WAITING;
    emit actionChanged();
}

void Monster::setDeath()
{
    m_stats[HPCUR] = 0;
    m_alive = false;
    emit hpChanged();
    emit aliveChanged();
    stopAllActions();
}

void Monster::stopAllActions()
{
    if (m_healer.isActive())
        m_healer.stop();
    if (m_starver.isActive())
        m_starver.stop();
    if (m_feeder.isActive())
        m_feeder.stop();
    if (m_trainer.isActive())
        m_trainer.stop();
    if (m_explorer.isActive())
        m_explorer.stop();
}

void Monster::starving()
{
    if (m_alive) {
        if (m_stats[HUNGERCUR] <= 0) {
            if (m_stats[HPCUR] > 0) {
                if (m_healer.isActive())
                    m_healer.stop();
                m_stats[HPCUR] -= 10;
                if (m_stats[HPCUR] <= 0)
                    return setDeath();
                emit hpChanged();
            }
            else {
                m_alive = false;
                emit aliveChanged();
            }
        }
        else
            m_stats[HUNGERCUR]--;
        setStarvingTime();
        emit hungerChanged();
    }
}

void Monster::healing()
{
    if (m_alive) {
        if (m_stats[HPCUR] < m_stats[HPMAX]) {
            if (m_action == RESTING) {
                m_stats[HPCUR] += 1 + m_stats[LEVEL];
                if (m_stats[HPCUR] > m_stats[HPMAX])
                    m_stats[HPCUR] = m_stats[HPMAX];
                m_curTime = m_stats[HPMAX] - m_stats[HPCUR];
                emit actionTimeChanged();
            }
            else
                m_stats[HPCUR]++;
        }
        else if (m_action == RESTING){
            m_action = WAITING;
            emit actionChanged();
        }
        if (m_action == RESTING)
            m_healer.start(1500);
        else
            m_healer.start(50000);
        emit hpChanged();
    }
}

void Monster::training()
{
    if (m_alive && m_action == TRAINING || m_action == ONBREAK) {
        if (m_action == TRAINING && m_stats[HUNGERCUR] <= 20 && m_stats[FOOD] > 0) {
            m_action = ONBREAK;
            m_starver.stop();
            emit actionChanged();
        }
        if (m_action == ONBREAK && m_stats[FOOD] > 0)
            setBreakTraining();
        else {
            if (m_action == ONBREAK) {
                m_action = TRAINING;
                m_starver.start(5000 + getStarvingTolerance());
                emit actionChanged();
            }
            m_stats[XPCUR] += 1 + getStrengthEfficiency(3);
            if (m_stats[XPCUR] == m_stats[XPMAX])
                levelUp();
            setTrainingTime();
            emit xpChanged();
            emit actionTimeChanged();
        }
    }
}

void Monster::exploring()
{
    if (m_alive && m_action == EXPLORING) {
        if (m_stats[LVLACTIONCUR] >= m_stats[LEVEL]) {
            m_stats[XPCUR] += 1 + m_stats[LVLACTIONCUR] - m_stats[LEVEL] + getStrengthEfficiency(3);
            m_stats[HPCUR] -= 1 + m_stats[LVLACTIONCUR] - m_stats[LEVEL];
            if (m_stats[HPCUR] <= 0)
                return setDeath();
        }
        else
            m_stats[XPCUR] += 1 + getStrengthEfficiency(3);
        if (m_stats[XPCUR] >= m_stats[XPMAX])
            levelUp();
        m_stats[FOOD] += QRandomGenerator::global()->generate() % 3 + getStrengthEfficiency(2);
        setExploringTime();
        emit xpChanged();
        emit foodGained();
        emit hpChanged();
        emit actionTimeChanged();
    }
}

void Monster::donjoning()
{
    if (m_alive && m_action == DONJONING) {
        m_stats[XPCUR] += 1 + getStrengthEfficiency(3);
        m_stats[HPCUR] -= 1 + m_stats[LVLACTIONCUR] * 2 - m_stats[LEVEL];
        if (m_stats[HPCUR] <= 0)
            return setDeath();
        if (m_stats[XPCUR] >= m_stats[XPMAX])
            levelUp();
        setDonjoningTime();
        emit xpChanged();
        emit hpChanged();
        emit actionTimeChanged();
    }
}

void Monster::feeding()
{
    if (m_alive && m_action == FEEDING && m_stats[FOOD] > 0 && m_stats[HUNGERCUR] < m_stats[HUNGERMAX]) {
        if (!m_healer.isActive())
            m_healer.start(10000);
        m_stats[HUNGERCUR]++;
        m_stats[FOOD]--;
        m_curTime--;
        setEatingTime();
        emit foodGained();
        emit hungerChanged();
        emit actionTimeChanged();
    }
}

void Monster::rest()
{
    if (m_action == WAITING) {
        m_action = RESTING;
        m_curTime = m_time = m_stats[HPMAX] - m_stats[HPCUR];
        m_healer.stop();
        m_healer.start(1000);
        emit actionChanged();
        emit actionTimeChanged();
    }
}

void Monster::feed(int food)
{
    if (m_action == WAITING) {
        m_time = m_curTime = food;
        m_action = FEEDING;
        if (m_starver.isActive())
            m_starver.stop();
        m_feeder.start(1000);
        emit actionChanged();
        emit actionTimeChanged();
    }
}

void Monster::train(int time)
{
    if (m_action == WAITING) {
        m_curTime = m_time = time;
        m_healer.stop();
        m_action = TRAINING;
        if (m_stats[HUNGERCUR] > 20)
            m_trainer.start(10000);
        else
            m_trainer.start(1000);
        emit actionChanged();
        emit actionTimeChanged();
    }
}

void Monster::explore(int level)
{
    if (m_action == WAITING) {
        int tmp = getExploringTotalTime(level);

        if (level == m_stats[LVLEXPLOMAX])
            m_stats[LVLEXPLOMAX]++;
        m_curTime = m_time = tmp - getOutSpeedEfficiency(tmp);//calcul time from level
        m_stats[LVLACTIONCUR] = level;
        m_healer.stop();
        m_action = EXPLORING;
        m_explorer.start(10000 - getInSpeedEfficiency());
        emit actionChanged();
        emit actionTimeChanged();
    }
}

void Monster::donjon(int level)
{
    if (m_action == WAITING) {
        int tmp = getExploringTotalTime(level);
        m_curTime = m_time = tmp - getOutSpeedEfficiency(tmp);//calcul time from level
        m_stats[LVLACTIONCUR] = level;
        m_healer.stop();
        m_action = DONJONING;
        m_donjoner.start(10000 - getInSpeedEfficiency());
        emit actionChanged();
        emit actionTimeChanged();
    }
}

std::shared_ptr<Equipement> Monster::getEquipement(const bool equiped, const int pos) const
{
    if (equiped)
        return m_wearing.at(pos);
    return m_inventory.at(pos);
}

void Monster::addEquipement(const std::shared_ptr<Equipement> item, const bool equiped, const int pos)
{
    if (item) {
        if (equiped) {
            m_wearing[pos] = item;
            m_stats[STRBOOST] += item->getStats(Equipement::STR);
            m_stats[ENDBOOST] += item->getStats(Equipement::END);
            m_stats[SPDBOOST] += item->getStats(Equipement::SPD);
            m_stats[HPMAX] = calculHP();
            m_stats[HUNGERMAX] = calculHunger();
            emit hpChanged();
            emit hungerChanged();
        }
        else
            m_inventory[pos] = item;
        emit equipementChanged();
    }
}

void Monster::setItemToSell(const int itemNb)
{
    m_itemToSell = itemNb;
}

void Monster::sellItem()
{
    std::shared_ptr<Equipement> del = m_inventory.at(m_itemToSell);

    if (del) {
        int gold = del->getStats(Equipement::STR);
        gold += del->getStats(Equipement::END);
        gold += del->getStats(Equipement::SPD);
        gold *= del->getRarityInt() * 10;
        m_stats[GOLD] += gold;
        removeItemFromInventory(del);
        emit equipementChanged();
    }
}

void Monster::spendPointOnStr()
{
    m_stats[TOPLACE]--;
    m_stats[STR]++;
    m_stats[HPMAX] = calculHP();
    emit hpChanged();
    emit statsToPlaceChanged();
    emit statsStrChanged();
}

void Monster::spendPointOnEnd()
{
    m_stats[TOPLACE]--;
    m_stats[END]++;
    m_stats[HUNGERMAX] = calculHunger();
    emit hungerChanged();
    emit statsToPlaceChanged();
    emit statsEndChanged();
}

void Monster::spendPointonSpd()
{
    m_stats[TOPLACE]--;
    m_stats[SPD]++;
    emit statsToPlaceChanged();
    emit statsSpdChanged();
}

void Monster::addItemToInventory(std::shared_ptr<Equipement> eq)
{
    bool ok = false;

    for (int i = 0; i < m_inventory.size(); i++) {
        if (m_inventory.at(i) == NULL) {
            ok = true;
            m_inventory[i] = eq;
            break;
        }
    }
    if (!ok)
        m_inventory.push_back(eq);
}

void Monster::removeItemFromInventory(std::shared_ptr<Equipement> eq)
{
    bool ok = false;

    for (int i = 0; i < m_inventory.size(); i++) {
        if ((m_inventory.at(i) == eq || ok == true) && i < m_inventory.size() - 1) {
            ok = true;
            m_inventory[i] = m_inventory.at(i + 1);
            m_inventory[i + 1] = NULL;
        }
    }
}

void Monster::gainDonjon()
{
    std::shared_ptr<Equipement> newEq = EquipementFabricator::Instance().getNewEquipement(m_stats[LVLACTIONCUR]);

    addItemToInventory(newEq);
    m_stats[GOLD] += calculDonjonGold();
    emit equipementChanged();
    emit goldGained();
}

void Monster::setEquipement(const MyLabel *clicked)
{
    std::shared_ptr<Equipement> toEquip = m_inventory.at(clicked->getPos());

    if (toEquip) {
        std::shared_ptr<Equipement> toUnEquip = m_wearing.at(toEquip->getTypeInt());
        removeItemFromInventory(toEquip);
        if (toUnEquip) {
            addItemToInventory(toUnEquip);
            m_stats[STRBOOST] -= toUnEquip->getStats(Equipement::STR);
            m_stats[ENDBOOST] -= toUnEquip->getStats(Equipement::END);
            m_stats[SPDBOOST] -= toUnEquip->getStats(Equipement::SPD);
        }
        m_wearing[toEquip->getTypeInt()] = toEquip;
        m_stats[STRBOOST] += toEquip->getStats(Equipement::STR);
        m_stats[ENDBOOST] += toEquip->getStats(Equipement::END);
        m_stats[SPDBOOST] += toEquip->getStats(Equipement::SPD);
        m_stats[HPMAX] = calculHP();
        if (m_stats[HPCUR] > m_stats[HPMAX])
            m_stats[HPCUR] = m_stats[HPMAX];
        m_stats[HUNGERMAX] = calculHunger();
        if (m_stats[HUNGERCUR] > m_stats[HUNGERMAX])
            m_stats[HUNGERCUR] = m_stats[HUNGERMAX];
        emit hpChanged();
        emit hungerChanged();
        emit equipementChanged();
    }
}

void Monster::unsetEquipement(const MyLabel *clicked)
{
    std::shared_ptr<Equipement> toUnEquip = m_wearing.at(clicked->getPos());

    if (toUnEquip) {
        addItemToInventory(toUnEquip);
        m_wearing[clicked->getPos()] = NULL;
        m_stats[STRBOOST] -= toUnEquip->getStats(Equipement::STR);
        m_stats[ENDBOOST] -= toUnEquip->getStats(Equipement::END);
        m_stats[SPDBOOST] -= toUnEquip->getStats(Equipement::SPD);
        m_stats[HPMAX] = calculHP();
        if (m_stats[HPCUR] > m_stats[HPMAX])
            m_stats[HPCUR] = m_stats[HPMAX];
        m_stats[HUNGERMAX] = calculHunger();
        if (m_stats[HUNGERCUR] > m_stats[HUNGERMAX])
            m_stats[HUNGERCUR] = m_stats[HUNGERMAX];
        emit hpChanged();
        emit hungerChanged();
        emit equipementChanged();
    }
}

QString Monster::getName()
{
    return m_name;
}

Monster::e_type Monster::getType()
{
    return m_type;
}

int Monster::getTypeInt()
{
    return m_type;
}

QString Monster::getTypeStr()
{
    QList<QString> tmp = {"Fire", "Water", "Earth", "Air", "Dark", "Light", "Arcane", "Chaos"};
    return tmp[m_type];
}

QString Monster::getActionStr()
{
    QList<QString> tmp = {"Waiting", "Resting", "Eating", "Training", "Training(Break)", "Exploring", "Donjoning"};
    return tmp[m_action];
}

int Monster::getTime()
{
    return m_time;
}

int Monster::getCurTime()
{
    return m_time - m_curTime;
}

bool Monster::isAlive()
{
    return m_alive;
}

void Monster::clearEquipement()
{
    for (int i = 0; i < 16; i++) {
        if (i < 4 && m_wearing[i] != NULL) {
            m_stats[STRBOOST] -= m_wearing[i]->getStats(Equipement::STR);
            m_stats[ENDBOOST] -= m_wearing[i]->getStats(Equipement::END);
            m_stats[SPDBOOST] -= m_wearing[i]->getStats(Equipement::SPD);
            m_wearing[i] = NULL;
        }
        else if (m_inventory[i] != NULL)
            m_inventory[i] = NULL;
    }
}

void Monster::update()
{
    emit levelChanged();
    emit xpChanged();
    emit hpChanged();
    emit hungerChanged();
    emit foodGained();
    emit actionChanged();
    emit statsToPlaceChanged();
    emit statsStrChanged();
    emit statsEndChanged();
    emit statsSpdChanged();
    emit equipementChanged();
}

int Monster::getStats(const e_stats stats)
{
    return m_stats[stats];
}

Monster::e_action Monster::getAction()
{
    return m_action;
}
