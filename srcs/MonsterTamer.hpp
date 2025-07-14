#pragma once

#include <QMainWindow>
#include <QObject>
#include <QPushButton>
#include <QSettings>
#include <QMenu>

#include "Monster.hpp"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MonsterTamer : public QObject
{
    Q_OBJECT

public:
    MonsterTamer();
    MonsterTamer(Ui::MainWindow  *ui);
    ~MonsterTamer();

    void setMonster(Ui::MainWindow  *ui);

    void setMenu();
    void setInventoryDisplay();
    void init();
    void createMonster();
    void getMonster(const QString name);
    void setMonsterDisplay();
    void connectMonsterSignals();
    void connectInventorySignals();
    void upDateMonsterInfo();
    void saveMonsterInfo();
    //QString get

public slots:
    void monsterLvlChanged();
    void monsterXpChanged();
    void monsterHpChanged();
    void monsterHungerChanged();
    void monsterStatsToPlaceChanged();
    void monsterStrChanged();
    void monsterEndChanged();
    void monsterSpdChanged();
    void monsterFoodGained();
    void monsterGoldGained();
    void monsterActionChanged();
    void monsterActionTimeChanged();
    void monsterAliveChanged();
    void feedMonster();
    void trainMonster();
    void exploreMonster();
    void donjonMonster();
    void saveMonster();
    void loadMonster();
    void resetMonster();
    void setFoodNb(const int value);
    void setTrainTime(const int value);
    void setExploreLevel(const int value);
    void setDonjonLevel(const int value);
    void displayItems();
    QString getItemStyleSheet(const int backGround, const int border);
    QString getItemImg(const int type);
    void displayItemStats(const MyLabel *place);
    void sellItem();

private:
    Ui::MainWindow              *m_ui;
    std::unique_ptr<Monster>    m_monster;
    QSettings                   m_save;
    int                         m_itemToSell;
    QMenu                       m_toolMenu;
    QAction                     m_saveAction;
    QAction                     m_loadAction;
    QAction                     m_resetAction;
    QList<MyLabel *>            m_inventory;

    void saveMonsterInventory(QSettings *settings);
    void saveOneItem(QSettings *settings, const QString group, int itemNb);
    void loadMonsterInventory(QSettings *settings);
    void loadOneItem(QSettings *settings, const QString group, int itemNb);
};
