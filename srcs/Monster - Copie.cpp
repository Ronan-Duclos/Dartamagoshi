#include "Monster.hpp"

#include <QtMath>
#include <QSettings>
#include <QDebug>

Monster::Monster(QString name)
{
    setNewMonster(name);
}

Monster::Monster(QString name, t_type type, int stats[11]) :
    m_name(name), m_type(type), m_level(stats[0]), m_xp{stats[1], stats[2]},
    m_hp{stats[3], stats[4]}, m_hunger{stats[5], stats[6]}, m_alive(true),
    m_stats{stats[7], stats[8], stats[9], stats[10]}, m_action(WAITING),
    m_boostedStats{0, 0, 0, 0}
{
    setInventory();
    setMonster();
}

Monster::~Monster()
{
    delete m_starver;
    delete m_healer;
    delete m_feeder;
    delete m_trainer;
    delete m_explorer;
    for (int i = 0; i < 16; i++) {
        if (i < 4 && m_wearing[i] != NULL)
            delete m_wearing[i];
        else if (m_inventory[i] != NULL)
            delete m_inventory[i];
    }
}

void Monster::setNewMonster(QString name)
{
    m_name = name;
    m_type = static_cast<t_type>(QRandomGenerator::global()->generate() % 8);
    m_level = 1;
    m_xp[0] = 0;
    m_xp[1] = 10;
    m_hp[0] = 100;
    m_hp[1] = 100;
    m_hunger[0] = 50;
    m_hunger[1] = 50;
    m_stats[TOPLACE] = 1;
    m_stats[STR] = 1;
    m_stats[END] = 1;
    m_stats[SPD] = 1;
    m_alive = true;
    setMonster();
}

void Monster::setMonster()
{
    m_starver = new QTimer(this);
    m_healer = new QTimer(this);
    m_feeder = new QTimer(this);
    m_trainer = new QTimer(this);
    m_explorer = new QTimer(this);
    connect(m_starver, &QTimer::timeout, this, &Monster::starving);
    connect(m_healer, &QTimer::timeout, this, &Monster::healing);
    connect(m_feeder, &QTimer::timeout, this, &Monster::feeding);
    connect(m_trainer, &QTimer::timeout, this, &Monster::training);
    connect(m_explorer, &QTimer::timeout, this, &Monster::exploring);
    m_starver->start(10000 + 1000 * (m_level - 1));
    m_healer->start(10000);
    m_action = WAITING;
}

void Monster::setInventory()
{
    Equipement *tmp = NULL;

    for (int i = 0; i < 4; i++)
        m_wearing.push_back(tmp);
    for (int i = 0; i < 16; i++)
        m_inventory.push_back(tmp);
}

void Monster::levelUp()
{
    m_xp[0] = 0;
    m_hp[0] = m_hp[1] = 100 + 10 * m_level + 20 * m_stats[STR];
    m_hunger[0] = m_hunger[1] = 50 + 10 * m_level + 20 * m_stats[END];
    if (m_level < 100)
        m_level++;
    m_xp[1] = 10 * qPow(m_level, 1.86);
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
    return 1000 + (m_level * 100) + (m_stats[END] * 200);
}

int Monster::getInSpeedEfficiency()
{
    return m_level * 100 + m_stats[SPD] * 200;
}

int Monster::getOutSpeedEfficiency(int total)
{
    return total * (m_level + m_stats[SPD]) / 100;
}

int Monster::getStrengthEfficiency(int divisor)
{
    return m_stats[STR] / divisor;
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
    m_starver->start(time);
}

void Monster::setEatingTime()
{
    if (m_hp[0] < m_hp[1]) {
        m_hp[0]++;
        emit hpChanged();
    }
    if (m_food == 0 || m_hunger[0] >= m_hunger[1]) {
        if (m_hunger[0] > m_hunger[1])
            m_hunger[0] = m_hunger[1];
        m_starver->start(10000 + getStarvingTolerance());
        m_feeder->stop();
        stopAction();
    }
    else
        m_feeder->start(1000);
}

void Monster::setTrainingTime()
{
    if (m_action == TRAINING) {
        if (m_curTime > 20) {
            m_trainer->start(20000 - getInSpeedEfficiency());
            m_curTime -= 20;
        }
        else if (m_curTime <= 0) {
            m_healer->start(10000);
            m_trainer->stop();
            stopAction();
        }
        else {
            m_trainer->start(m_curTime * 1000);
            m_curTime = 0;
        }
    }
    else {
        if (m_curTime > 1) {
            m_trainer->start(1000);
            m_curTime -= 1;
        }
        else {
            m_healer->start(10000);
            m_trainer->stop();
            stopAction();
        }
    }
}

void Monster::setBreakTraining()
{
    emit takenFood();
    m_hunger[0]++;
    m_curTime -= 1;
    setTrainingTime();
    emit hungerChanged();
    emit actionTimeChanged();
    if (m_hunger[0] == m_hunger[1]) {
        m_action = TRAINING;
        m_starver->start(5000 + getStarvingTolerance());
        emit actionChanged();
    }
}

void Monster::setExploringTime()
{
    if (m_curTime > 10)
        m_explorer->start(10000 - getInSpeedEfficiency());
    else if (m_curTime < 0) {
        m_healer->start(10000);
        m_explorer->stop();
        stopAction();
        emit foodGained();
    }
    else
        m_explorer->start(m_curTime * 1000);
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
    m_hp[0] = 0;
    m_alive = false;
    emit hpChanged();
    emit aliveChanged();
    stopAllActions();
}

void Monster::stopAllActions()
{
    if (m_healer->isActive())
        m_healer->stop();
    if (m_starver->isActive())
        m_starver->stop();
    if (m_feeder->isActive())
        m_feeder->stop();
    if (m_trainer->isActive())
        m_trainer->stop();
    if (m_explorer->isActive())
        m_explorer->stop();
}

void Monster::starving()
{
    if (m_alive) {
        if (m_hunger[0] <= 0) {
            if (m_hp[0] > 0) {
                if (m_healer->isActive())
                    m_healer->stop();
                m_hp[0] -= 10;
                if (m_hp[0] <= 0)
                    return setDeath();
                emit hpChanged();
            }
            else {
                m_alive = false;
                emit aliveChanged();
            }
        }
        else
            m_hunger[0]--;
        setStarvingTime();
        emit hungerChanged();
    }
}

void Monster::healing()
{
    if (m_alive) {
        if (m_hp[0] < m_hp[1]) {
            if (m_action == RESTING) {
                m_hp[0] += 1 + m_level;
                if (m_hp[0] > m_hp[1])
                    m_hp[0] = m_hp[1];
            }
            else
                m_hp[0]++;
        }
        else if (m_action == RESTING){
            m_action = WAITING;
            emit actionChanged();
        }
        if (m_action == RESTING)
            m_healer->start(1500);
        else
            m_healer->start(50000);
        emit hpChanged();
    }
}

void Monster::training()
{
    if (m_alive && m_action == TRAINING || m_action == ONBREAK) {
        if (m_action == TRAINING && m_hunger[0] <= 20 && m_foodAvailable) {
            m_action = ONBREAK;
            m_starver->stop();
            emit actionChanged();
        }
        if (m_action == ONBREAK && m_foodAvailable)
            setBreakTraining();
        else {
            if (m_action == ONBREAK) {
                m_action = TRAINING;
                m_starver->start(5000 + getStarvingTolerance());
                emit actionChanged();
            }
            m_xp[0] += 1 + getStrengthEfficiency(3);
            if (m_xp[0] == m_xp[1])
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
        if (m_levelExplore >= m_level) {
            m_xp[0] += 1 + m_levelExplore - m_level + getStrengthEfficiency(3);
            m_hp[0] -= 1 + m_levelExplore - m_level;
            if (m_hp[0] <= 0)
                return setDeath();
        }
        else
            m_xp[0] += 1 + getStrengthEfficiency(3);
        if (m_xp[0] >= m_xp[1])
            levelUp();
        m_foodGained += QRandomGenerator::global()->generate() % 3 + getStrengthEfficiency(2);
        m_curTime -= 10;
        setExploringTime();
        emit xpChanged();
        emit hpChanged();
        emit actionTimeChanged();
    }
}

void Monster::feeding()
{
    if (m_alive && m_action == FEEDING && m_food > 0 && m_hunger[0] < m_hunger[1]) {
        if (!m_healer->isActive())
            m_healer->start(10000);
        m_hunger[0]++;
        m_food--;
        m_curTime--;
        setEatingTime();
        emit hungerChanged();
        emit actionTimeChanged();
    }
}

void Monster::train(int time)
{
    if (m_action == WAITING) {
        m_curTime = m_time = time;
        m_healer->stop();
        m_action = TRAINING;
        if (m_hunger[0] > 20)
            m_trainer->start(10000);
        else
            m_trainer->start(1000);
        emit actionChanged();
        emit actionTimeChanged();
    }
}

void Monster::explore(int level)
{
    if (m_action == WAITING) {
        int tmp = getExploringTotalTime(level);
        m_curTime = m_time = tmp - getOutSpeedEfficiency(tmp);//calcul time from level
        qDebug() << "Starting to explor for" << m_time << "secondes";
        m_levelExplore = level;
        m_healer->stop();
        m_action = EXPLORING;
        m_foodGained = 0;
        m_explorer->start(5000);
        emit actionChanged();
        emit actionTimeChanged();
    }
}

void Monster::feed(int food)
{
    if (m_action == WAITING) {
        m_time = m_curTime = m_food = food;
        m_action = FEEDING;
        if (m_starver->isActive())
            m_starver->stop();
        m_feeder->start(1000);
        emit actionChanged();
        emit actionTimeChanged();
    }
}

Equipement *Monster::getEquipement(bool equiped, int pos)
{
    if (equiped)
        return m_wearing.at(pos);
    return m_inventory.at(pos);
}

void Monster::addEquipement(Equipement *item, bool equiped, int pos)
{
    if (item) {
        if (equiped)
            m_wearing.at(pos) = item;
        else
            m_inventory.at(pos) = item;
    }
}

void Monster::rest()
{
    if (m_action == WAITING) {
        m_action = RESTING;
        m_healer->stop();
        m_healer->start(1000);
        emit actionChanged();
    }
}

/*void Monster::Battle()
{

}

void Monster::clean()
{

}*/

void Monster::spendPointOnStr()
{
    m_stats[TOPLACE]--;
    m_stats[STR]++;
    m_hp[1] = 100 + 10 * m_level + 20 * m_stats[STR];
    emit hpChanged();
    emit statsToPlaceChanged();
    emit statsStrChanged();
}

void Monster::spendPointOnEnd()
{
    m_stats[TOPLACE]--;
    m_stats[END]++;
    m_hunger[1] = 50 + 10 * m_level + 20 * m_stats[END];
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

void Monster::addItemToInventory(Equipement *eq)
{
    bool ok = false;

    for (int i = 0; i < m_inventory.size(); i++) {
        if (m_inventory.at(i) == NULL) {
            ok = true;
            m_inventory.at(i) = eq;
        }
    }
    if (!ok)
        m_inventory.push_back(eq);
}

void Monster::removeItemFromInventory(Equipement *eq)
{
    bool ok = false;

    for (int i = 0; i < m_inventory.size(); i++) {
        if ((m_inventory.at(i) == eq || ok == true) && i < m_inventory.size() - 1) {
            ok = true;
            m_inventory.at(i) = m_inventory.at(i + 1);
            m_inventory.at(i + 1) = NULL;
        }
    }
}

void Monster::setEquipement(MyLabel *clicked)
{
    Equipement *toEquip = m_inventory.at(clicked->getPos());

    if (toEquip) {
        Equipement *toUnEquip = m_wearing.at(toEquip->getTypeInt());
        removeItemFromInventory(toEquip);
        if (toUnEquip) {
            addItemToInventory(toUnEquip);
            m_boostedStats[STR] -= toUnEquip->getStr();
            m_boostedStats[END] -= toUnEquip->getEnd();
            m_boostedStats[SPD] -= toUnEquip->getSpd();
        }
        m_wearing.at(toEquip->getTypeInt()) = toEquip;
        m_boostedStats[STR] += toEquip->getStr();
        m_boostedStats[END] += toEquip->getEnd();
        m_boostedStats[SPD] += toEquip->getSpd();
        emit equipementChanged();
    }
}

void Monster::unsetEquipement(MyLabel *clicked)
{
    Equipement *toUnEquip = m_wearing.at(clicked->getPos());

    if (toUnEquip) {
        addItemToInventory(toUnEquip);
        m_wearing.at(clicked->getPos()) = NULL;
        m_boostedStats[STR] -= toUnEquip->getStr();
        m_boostedStats[END] -= toUnEquip->getEnd();
        m_boostedStats[SPD] -= toUnEquip->getSpd();
        emit equipementChanged();
    }
}

QString Monster::getName()
{
    return m_name;
}

Monster::t_type Monster::getType()
{
    return m_type;
}

int Monster::getTypeInt()
{
    return m_type;
}

QString Monster::getTypeStr()
{
    std::vector<QString> tmp = {"Fire", "Water", "Earth", "Air", "Dark", "Light", "Arcane", "Chaos"};
    return tmp[m_type];
}

QString Monster::getActionStr()
{
    std::vector<QString> tmp = {"Waiting", "Resting", "Eating", "Training", "Training(Break)", "Exploring"};
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

void Monster::update()
{
    emit levelChanged();
    emit xpChanged();
    emit hpChanged();
    emit hungerChanged();
    emit actionChanged();
    emit statsToPlaceChanged();
    emit statsStrChanged();
    emit statsEndChanged();
    emit statsSpdChanged();
}

int Monster::getFoodGained()
{
    /*int ret = m_foodGained;
    m_foodGained = 0;
    return ret;*/
    return m_foodGained;
}

void Monster::setFoodAvailable(bool available)
{
    m_foodAvailable = available;
}

int Monster::getLevel()
{
    return m_level;
}

int *Monster::getXp()
{
    return m_xp;
}

int *Monster::getHp()
{
    return m_hp;
}

int *Monster::getHunger()
{
    return m_hunger;
}

int Monster::getStatsToPlace()
{
    return m_stats[TOPLACE];
}

int Monster::getStr()
{
    return m_stats[STR];
}

int Monster::getEnd()
{
    return m_stats[END];
}

int Monster::getSpd()
{
    return m_stats[SPD];
}

int Monster::getStatsBonus(t_stats stats)
{
    return m_boostedStats[stats];
}

Monster::t_action Monster::getAction()
{
    return m_action;
}
