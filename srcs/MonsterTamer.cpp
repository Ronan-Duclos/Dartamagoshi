#include "MonsterTamer.hpp"
#include "../ui_mainwindow.h"

#include <QMessageBox>
#include <QInputDialog>
#include <QFileDialog>
#include <QPixmap>
#include <QDial>
#include <MyLabel.hpp>

#include "EquipementFabricator.hpp"

MonsterTamer::MonsterTamer() :
    m_save("DartamaGoshi", "CanaSkySoftware"), m_toolMenu("Files"),
    m_saveAction(QAction(QIcon("img/icons/saveIcon.jpg"), "Save")),
    m_loadAction(QAction(QIcon("img/icons/loadIcon.jpg"), "Load")),
    m_resetAction(QAction(QIcon("img/icons/resetIcon.jpg"), "Reset"))
{

}

MonsterTamer::MonsterTamer(Ui::MainWindow *ui) : m_ui(ui),
    m_save("DartamaGoshi", "CanaSkySoftware"), m_toolMenu("Files"),
    m_saveAction(QAction(QIcon("img/icons/saveIcon.jpg"), "Save")),
    m_loadAction(QAction(QIcon("img/icons/loadIcon.jpg"), "Load")),
    m_resetAction(QAction(QIcon("img/icons/resetIcon.jpg"), "Reset"))

{
    setMenu();
    setInventoryDisplay();
    init();
    displayItems();
}

MonsterTamer::~MonsterTamer()
{

}

void MonsterTamer::setMonster(Ui::MainWindow  *ui)
{
    m_ui = ui;
    setMenu();
    setInventoryDisplay();
    init();
    displayItems();
}

void MonsterTamer::setMenu()
{
    m_toolMenu.addAction(&m_saveAction);
    m_toolMenu.addAction(&m_loadAction);
    m_toolMenu.addAction(&m_resetAction);
    m_ui->menuBar->addMenu(&m_toolMenu);

    connect(&m_saveAction, &QAction::triggered, this, &MonsterTamer::saveMonster);
    connect(&m_loadAction, &QAction::triggered, this, &MonsterTamer::loadMonster);
    connect(&m_resetAction, &QAction::triggered, this, &MonsterTamer::resetMonster);
}

void MonsterTamer::setInventoryDisplay()
{
    m_inventory = {m_ui->clawsWeapon, m_ui->fangsWeapon, m_ui->headArmor, m_ui->chestArmor,
                  m_ui->item_01, m_ui->item_02, m_ui->item_03, m_ui->item_04, m_ui->item_05,
                  m_ui->item_06, m_ui->item_07, m_ui->item_08, m_ui->item_09, m_ui->item_10,
                  m_ui->item_11, m_ui->item_12, m_ui->item_13, m_ui->item_14, m_ui->item_15, m_ui->item_16, };


    for (int i = 0; i < m_inventory.size(); i++) {
        if (i < 4) {
            m_inventory[i]->setPos(i);
            m_inventory[i]->setEquiped(true);
        }
        else
            m_inventory[i]->setPos(i - 4);
    }
}

void MonsterTamer::init()
{
    QString name = m_save.value("Name").toString();

    if (name.isEmpty())
        createMonster();
    else
        getMonster(name);
    setMonsterDisplay();
    connectMonsterSignals();
}

void MonsterTamer::createMonster()
{
    QString name;
    bool ok;
    QString imgPath;

    while (!ok || name.isEmpty()) {
        int tmp = QRandomGenerator::global()->generate() % 12;
        imgPath = QString("img/signes/" + QString::number(tmp) + ".png");
        QPixmap img(imgPath);

        m_ui->monsterImg->setPixmap(img);
        name = QInputDialog::getText(m_ui->centralwidget,
                                     tr("Monster Name"),
                                     tr("Name your new monster"),
                                     QLineEdit::Normal, "", &ok);
    }
    m_monster = std::make_unique<Monster>(name);
    m_save.setValue("Name", name);
    m_save.setValue("Type", m_monster->getTypeInt());
    m_save.setValue("Img", imgPath);
    m_save.setValue("food", m_monster->getStats(Monster::FOOD));
    m_ui->exploreLvl->setMaximum(1);
    m_ui->exploreLvlDial->setMaximum(1);
    m_ui->donjonLvl->setMaximum(1);
    m_ui->donjonLvlDial->setMaximum(1);
    saveMonsterInfo();
}

void MonsterTamer::getMonster(const QString name)
{
    int stats[14];
    QPixmap img(m_save.value("Img").toString());
    int type = m_save.value("type").toInt();
    stats[0] = m_save.value("level").toInt();
    stats[1] = m_save.value("xpMin").toInt();
    stats[2] = m_save.value("xpMax").toInt();
    stats[3] = m_save.value("hpMin").toInt();
    stats[4] = m_save.value("hpMax").toInt();
    stats[5] = m_save.value("hungerMin").toInt();
    stats[6] = m_save.value("hungerMax").toInt();
    stats[7] = m_save.value("statsToPlace").toInt();
    stats[8] = m_save.value("str").toInt();
    stats[9] = m_save.value("end").toInt();
    stats[10] = m_save.value("spd").toInt();
    stats[11] = m_save.value("food").toInt();
    stats[12] = m_save.value("gold").toInt();
    stats[13] = m_save.value("lvlMaxExplore").toInt();
    m_monster = std::make_unique<Monster>(name, static_cast<Monster::e_type>(type), stats);
    loadMonsterInventory(&m_save);
    m_ui->exploreLvl->setMaximum(stats[13]);
    m_ui->exploreLvl->setValue(stats[13]);
    m_ui->exploreLvlDial->setMaximum(stats[13]);
    m_ui->exploreLvlDial->setValue(stats[13]);
    m_ui->donjonLvl->setMaximum(stats[0]);
    m_ui->donjonLvlDial->setMaximum(stats[0]);
    m_ui->donjonLvl->setValue(stats[0]);
    m_ui->donjonLvlDial->setValue(stats[0]);
    m_ui->monsterImg->setPixmap(img);
}

void MonsterTamer::setMonsterDisplay()
{
    QString styleSheet1 = "border: 2px solid; border-radius: 40px;";
    QString styleSheet2 = "border: 2px solid; border-radius: 15px;";
    QString styleSheet3 = "border-radius: 40px;";
    QString styleSheet4[] = {" color: rgb(255, 255, 255); background-color: qconicalgradient(cx:0, cy:0, angle:135, stop:0 rgba(255, 0, 0, 69), stop:0.375 rgba(255, 0, 0, 69), stop:0.423533 rgba(251, 0, 0, 145), stop:0.45 rgba(247,0, 0, 208), stop:0.477581 rgba(255, 6, 0, 130), stop:0.518717 rgba(255, 0, 0, 130), stop:0.55 rgba(255, 0, 0, 255), stop:0.57754 rgba(255, 0, 0, 130), stop:0.625 rgba(255, 0, 0, 69), stop:1 rgba(255, 0, 0, 69));",
                             " color: rgb(255, 255, 255); background-color: qconicalgradient(cx:0, cy:0, angle:135, stop:0 rgba(50, 180, 255, 69), stop:0.375 rgba(50, 180, 255, 69), stop:0.423533 rgba(50, 180, 251, 145), stop:0.45 rgba(50, 180, 247, 208), stop:0.477581 rgba(50, 180, 255, 130), stop:0.518717 rgba(50, 180, 255, 130), stop:0.55 rgba(50, 180, 255, 255), stop:0.57754 rgba(50, 180, 255, 130), stop:0.625 rgba(50, 180, 255, 69), stop:1 rgba(50, 180, 255, 69));",
                             " color: rgb(255, 255, 255); background-color: qconicalgradient(cx:0, cy:0, angle:135, stop:0 rgba(120, 80, 20, 69), stop:0.375 rgba(120, 80, 20, 69), stop:0.423533 rgba(128, 80, 20, 145), stop:0.45 rgba(130, 80, 20, 208), stop:0.477581 rgba(120, 80, 20, 130), stop:0.518717 rgba(120, 80, 20, 130), stop:0.55 rgba(120, 80, 20, 255), stop:0.57754 rgba(120, 80, 20, 130), stop:0.625 rgba(120, 80, 20, 69), stop:1 rgba(120, 80, 20, 69));",
                             " color: rgb(0, 0, 0); background-color: qconicalgradient(cx:0, cy:0, angle:135, stop:0 rgba(180, 180, 255, 69), stop:0.375 rgba(180, 180, 255, 69), stop:0.423533 rgba(187, 187, 247, 145), stop:0.45 rgba(180, 180, 251, 208), stop:0.477581 rgba(180, 180, 255, 130), stop:0.518717 rgba(180, 180, 255, 130), stop:0.55 rgba(180, 180, 255, 255), stop:0.57754 rgba(180, 180, 255, 130), stop:0.625 rgba(180, 180, 255, 69), stop:1 rgba(180, 180, 255, 69));",
                             " color: rgb(0, 0, 0); background-color: qconicalgradient(cx:0, cy:0, angle:135, stop:0 rgba(0, 0, 0, 69), stop:0.375 rgba(0, 0, 0, 69), stop:0.423533 rgba(10, 10, 10, 145), stop:0.45 rgba(12, 12, 12, 208), stop:0.477581 rgba(0, 0, 0, 130), stop:0.518717 rgba(10, 10, 10, 130), stop:0.55 rgba(12, 12, 12, 255), stop:0.57754 rgba(10, 10, 10, 130), stop:0.625 rgba(0, 0, 0, 69), stop:1 rgba(0, 0, 0, 69));",
                             " color: rgb(0, 0, 0); background-color: qconicalgradient(cx:0, cy:0, angle:135, stop:0 rgba(255, 255, 0, 69), stop:0.375 rgba(255, 255, 0, 69), stop:0.423533 rgba(251, 255, 0, 145), stop:0.45 rgba(247, 255, 0, 208), stop:0.477581 rgba(255, 244, 71, 130), stop:0.518717 rgba(255, 218, 71, 130), stop:0.55 rgba(255, 255, 0, 255), stop:0.57754 rgba(255, 203, 0, 130), stop:0.625 rgba(255, 255, 0, 69), stop:1 rgba(255, 255, 0, 69));;",
                             " color: rgb(255, 255, 250); background-color: qconicalgradient(cx:0, cy:0, angle:135, stop:0 rgba(130, 70, 255, 69), stop:0.375 rgba(130, 70, 255, 69), stop:0.423533 rgba(130, 70, 251, 145), stop:0.45 rgba(130, 70, 247, 208), stop:0.477581 rgba(130, 70, 255, 130), stop:0.518717 rgba(130, 70, 255, 130), stop:0.55 rgba(130, 70, 255, 255), stop:0.57754 rgba(130, 70, 255, 130), stop:0.625 rgba(130, 70, 255, 69), stop:1 rgba(130, 70, 255, 69));",
                             " color: rgb(255, 255, 250); background-color: qconicalgradient(cx:0, cy:0, angle:135, stop:0 rgba(80, 10, 10, 69), stop:0.375 rgba(88, 10, 10, 69), stop:0.423533 rgba(75, 10, 10, 145), stop:0.45 rgba(92, 10, 10, 208), stop:0.477581 rgba(80, 10, 10, 130), stop:0.518717 rgba(80, 10, 10, 130), stop:0.55 rgba(80, 10, 10, 255), stop:0.57754 rgba(80, 10, 10, 130), stop:0.625 rgba(80, 10, 10, 69), stop:1 rgba(80, 10, 10, 69));"
                            };
    QString styleSheet5[] = {" background-color: rgb(220, 20, 20); color: rgb(255, 255, 255);",
                             " background-color: rgb(50, 180, 255); color: rgb(255, 255, 255);",
                             " background-color: rgb(120, 80, 20); color: rgb(255, 255, 255);",
                             " background-color: rgb(180, 180, 255); color: rgb(0, 0, 0);",
                             " background-color: rgb(0, 0, 0); color: rgb(100, 100, 100);",
                             " background-color: rgb(220, 220, 0); color: rgb(0, 0, 0);",
                             " background-color: rgb(130, 70, 255); color: rgb(255, 255, 255);",
                             " background-color: rgb(80, 10, 10); color: rgb(255, 255, 255);"
                            };
    QString styleSheet6[] = {" background-color: qradialgradient(spread:pad, cx:0.5, cy:0.5, radius:0.5, fx:0.5, fy:0.5, stop:0 rgba(0, 0, 0, 0), stop:0.52 rgba(0, 0, 0, 0), stop:0.565 rgba(220, 20, 20, 33), stop:0.65 rgba(220, 20, 20, 64), stop:0.721925 rgba(220, 20, 20, 129), stop:0.77 rgba(220, 20, 20, 204), stop:0.89 rgba(220, 20, 20, 64), stop:1 rgba(0, 0, 0, 0));",
                             " background-color: qradialgradient(spread:pad, cx:0.5, cy:0.5, radius:0.5, fx:0.5, fy:0.5, stop:0 rgba(0, 0, 0, 0), stop:0.52 rgba(0, 0, 0, 0), stop:0.565 rgba(50, 180, 255, 33), stop:0.65 rgba(50, 180, 255, 64), stop:0.721925 rgba(50, 180, 255, 129), stop:0.77 rgba(50, 180, 255, 204), stop:0.89 rgba(50, 180, 255, 64), stop:1 rgba(0, 0, 0, 0));",
                             " background-color: qradialgradient(spread:pad, cx:0.5, cy:0.5, radius:0.5, fx:0.5, fy:0.5, stop:0 rgba(0, 0, 0, 0), stop:0.52 rgba(0, 0, 0, 0), stop:0.565 rgba(120, 80, 20, 33), stop:0.65 rgba(120, 80, 20, 64), stop:0.721925 rgba(120, 80, 20, 129), stop:0.77 rgba(120, 80, 20, 204), stop:0.89 rgba(120, 80, 20, 64), stop:1 rgba(0, 0, 0, 0));",
                             " background-color: qradialgradient(spread:pad, cx:0.5, cy:0.5, radius:0.5, fx:0.5, fy:0.5, stop:0 rgba(0, 0, 0, 0), stop:0.52 rgba(0, 0, 0, 0), stop:0.565 rgba(180, 180, 255, 33), stop:0.65 rgba(180, 180, 255, 64), stop:0.721925 rgba(180, 180, 255, 129), stop:0.77 rgba(180, 180, 205, 254), stop:0.89 rgba(180, 180, 255, 64), stop:1 rgba(0, 0, 0, 0));",
                             " background-color: qradialgradient(spread:pad, cx:0.5, cy:0.5, radius:0.5, fx:0.5, fy:0.5, stop:0 rgba(0, 0, 0, 0), stop:0.52 rgba(0, 0, 0, 0), stop:0.565 rgba(0, 0, 0, 33), stop:0.65 rgba(0, 0, 0, 64), stop:0.721925 rgba(0, 0, 0, 129), stop:0.77 rgba(0, 0, 0, 204), stop:0.89 rgba(0, 0, 0, 64), stop:1 rgba(0, 0, 0, 0));",
                             " background-color: qradialgradient(spread:pad, cx:0.5, cy:0.5, radius:0.5, fx:0.5, fy:0.5, stop:0 rgba(0, 0, 0, 0), stop:0.52 rgba(0, 0, 0, 0), stop:0.565 rgba(220, 220, 0, 33), stop:0.65 rgba(220, 220, 0, 64), stop:0.721925 rgba(220, 220, 0, 129), stop:0.77 rgba(220, 220, 0, 204), stop:0.89 rgba(220, 220, 0, 64), stop:1 rgba(0, 0, 0, 0));",
                             " background-color: qradialgradient(spread:pad, cx:0.5, cy:0.5, radius:0.5, fx:0.5, fy:0.5, stop:0 rgba(0, 0, 0, 0), stop:0.52 rgba(0, 0, 0, 0), stop:0.565 rgba(130, 70, 255, 33), stop:0.65 rgba(130, 70, 255, 64), stop:0.721925 rgba(130, 70, 255, 129), stop:0.77 rgba(130, 70, 255, 204), stop:0.89 rgba(130, 70, 255, 64), stop:1 rgba(0, 0, 0, 0));",
                             " background-color: qradialgradient(spread:pad, cx:0.5, cy:0.5, radius:0.5, fx:0.5, fy:0.5, stop:0 rgba(0, 0, 0, 0), stop:0.52 rgba(0, 0, 0, 0), stop:0.565 rgba(80, 10, 10, 33), stop:0.65 rgba(80, 10, 10, 64), stop:0.721925 rgba(80, 10, 10, 129), stop:0.77 rgba(80, 10, 10, 204), stop:0.89 rgba(80, 10, 10, 64), stop:1 rgba(0, 0, 0, 0));"
                            };


    monsterLvlChanged();
    monsterXpChanged();
    monsterHpChanged();
    monsterHungerChanged();
    monsterActionChanged();
    int food = m_monster->getStats(Monster::FOOD);
    int hunger = m_monster->getStats(Monster::HUNGERMAX) - m_monster->getStats(Monster::HUNGERCUR);
    int gold = m_monster->getStats(Monster::GOLD);
    m_ui->foodNb->setMaximum(1);
    m_ui->foodNb->setMaximum(hunger);
    m_ui->foodNbDial->setMinimum(1);
    m_ui->foodNbDial->setMaximum(hunger);
    m_ui->tamerGold->setMinimumHeight(0);
    m_ui->tamerGold->display(gold);
    m_ui->tamerFood->display(food);
    m_ui->monsterName->setText(m_monster->getName());
    m_ui->monsterType->setText(m_monster->getTypeStr());
    m_ui->monsterName->setStyleSheet(styleSheet1 + styleSheet4[m_monster->getType()]);
    m_ui->monsterType->setStyleSheet(styleSheet2 + styleSheet5[m_monster->getType()]);
    m_ui->monsterImg->setStyleSheet(styleSheet3 + styleSheet6[m_monster->getType()]);
}

void MonsterTamer::connectMonsterSignals()
{
    // Monster To Tamer
    connect(m_monster.get(), &Monster::levelChanged, this, &MonsterTamer::monsterLvlChanged);
    connect(m_monster.get(), &Monster::xpChanged, this, &MonsterTamer::monsterXpChanged);
    connect(m_monster.get(), &Monster::hpChanged, this, &MonsterTamer::monsterHpChanged);
    connect(m_monster.get(), &Monster::hungerChanged, this, &MonsterTamer::monsterHungerChanged);
    connect(m_monster.get(), &Monster::foodGained, this, &MonsterTamer::monsterFoodGained);
    connect(m_monster.get(), &Monster::goldGained, this, &MonsterTamer::monsterGoldGained);
    connect(m_monster.get(), &Monster::actionChanged, this, &MonsterTamer::monsterActionChanged);
    connect(m_monster.get(), &Monster::actionTimeChanged, this, &MonsterTamer::monsterActionTimeChanged);
    connect(m_monster.get(), &Monster::aliveChanged, this, &MonsterTamer::monsterAliveChanged);
    connect(m_monster.get(), &Monster::statsToPlaceChanged, this, &MonsterTamer::monsterStatsToPlaceChanged);
    connect(m_monster.get(), &Monster::statsStrChanged, this, &MonsterTamer::monsterStrChanged);
    connect(m_monster.get(), &Monster::statsEndChanged, this, &MonsterTamer::monsterEndChanged);
    connect(m_monster.get(), &Monster::statsSpdChanged, this, &MonsterTamer::monsterSpdChanged);

    //Tamer To Monster
    connect(m_ui->restButton, &QPushButton::clicked, m_monster.get(), &Monster::rest);
    connect(m_ui->feedButton, &QPushButton::clicked, this, &MonsterTamer::feedMonster);
    connect(m_ui->trainButton, &QPushButton::clicked, this, &MonsterTamer::trainMonster);
    connect(m_ui->exploreButton, &QPushButton::clicked, this, &MonsterTamer::exploreMonster);
    connect(m_ui->donjonButton, &QPushButton::clicked, this, &MonsterTamer::donjonMonster);
    connect(m_ui->addStr, &QPushButton::clicked, m_monster.get(), &Monster::spendPointOnStr);
    connect(m_ui->addEnd, &QPushButton::clicked, m_monster.get(), &Monster::spendPointOnEnd);
    connect(m_ui->addSpd, &QPushButton::clicked, m_monster.get(), &Monster::spendPointonSpd);
    connect(m_ui->sellButton, &QPushButton::clicked, this, &MonsterTamer::sellItem);

    //Graphics To Graphics
    connect(m_ui->foodNbDial, &QDial::valueChanged, this, &MonsterTamer::setFoodNb);
    connect(m_ui->trainingTimeDial, &QDial::valueChanged, this, &MonsterTamer::setTrainTime);
    connect(m_ui->exploreLvlDial, &QDial::valueChanged, this, &MonsterTamer::setExploreLevel);
    connect(m_ui->donjonLvlDial, &QDial::valueChanged, this, &MonsterTamer::setDonjonLevel);

    connectInventorySignals();
}

void MonsterTamer::connectInventorySignals()
{
    connect(m_monster.get(), &Monster::equipementChanged, this, &MonsterTamer::displayItems);

    for (int i = 0; i < m_inventory.size(); i++) {
        if (i < 4)
            connect(m_inventory.at(i), &MyLabel::doubleClicked, m_monster.get(), &Monster::unsetEquipement);
        else
            connect(m_inventory.at(i), &MyLabel::doubleClicked, m_monster.get(), &Monster::setEquipement);
        connect(m_inventory.at(i), &MyLabel::clicked, this, &MonsterTamer::displayItemStats);
    }
}

void MonsterTamer::upDateMonsterInfo()
{
    m_monster->update();
}

void MonsterTamer::saveMonsterInfo()
{
    monsterLvlChanged();
    monsterXpChanged();
    monsterHpChanged();
    monsterHungerChanged();
    monsterStatsToPlaceChanged();
    monsterStrChanged();
    monsterEndChanged();
    monsterSpdChanged();
    m_save.setValue("lvlMaxExplore", m_monster->getStats(Monster::LVLEXPLOMAX));
}

void MonsterTamer::saveMonsterInventory(QSettings *settings)
{
    QString toSave[20] = {"Claws", "Fangs", "HeadArmor", "ChestArmor", "Item01", "Item02", "Item03",
                          "Item04", "Item05", "Item06", "Item07", "Item08", "Item09", "Item10",
                          "Item11", "Item12", "Item13", "Item14", "Item15", "Item16"};

    for (int i = 0; i < 20; i++) {
        settings->remove(toSave[i]);
        saveOneItem(settings, toSave[i], i);
    }
}

void MonsterTamer::saveOneItem(QSettings *settings, const QString group, int itemNb)
{
    std::shared_ptr<Equipement> item;
    bool equiped = true;

    if (itemNb >= 4) {
        equiped = false;
        itemNb -= 4;
    }
    item = m_monster->getEquipement(equiped, itemNb);
    if (item) {
        settings->beginGroup(group);
        settings->setValue("rarity", item->getRarityInt());
        settings->setValue("type", item->getTypeInt());
        settings->setValue("level", item->getStats(Equipement::LEVEL));
        settings->setValue("str", item->getStats(Equipement::STR));
        settings->setValue("end", item->getStats(Equipement::END));
        settings->setValue("spd", item->getStats(Equipement::SPD));
        settings->endGroup();
    }
}

void MonsterTamer::loadMonsterInventory(QSettings *settings)
{
    QString toSave[20] = {"Claws", "Fangs", "HeadArmor", "ChestArmor", "Item01", "Item02", "Item03",
                          "Item04", "Item05", "Item06", "Item07", "Item08", "Item09", "Item10",
                          "Item11", "Item12", "Item13", "Item14", "Item15", "Item16"};

    for (int i = 0; i < 20; i++)
        loadOneItem(settings, toSave[i], i);
}

void MonsterTamer::loadOneItem(QSettings *settings, const QString group, int itemNb)
{
    std::shared_ptr<Equipement> tmp;
    bool equiped = true;

    if (settings->childGroups().contains(group)) {
        int stats[4];
        settings->beginGroup(group);
        Equipement::e_rarity rarity = static_cast<Equipement::e_rarity>(settings->value("rarity").toInt());
        Equipement::e_type type = static_cast<Equipement::e_type>(settings->value("type").toInt());
        stats[0] = settings->value("level").toInt();
        stats[1] = settings->value("str").toInt();
        stats[2] = settings->value("end").toInt();
        stats[3] = settings->value("spd").toInt();

        tmp = EquipementFabricator::Instance().getEquipement(rarity, type, stats);
        if (itemNb >= 4) {
            equiped = false;
            itemNb -= 4;
        }
        m_monster->addEquipement(tmp, equiped, itemNb);
        settings->endGroup();
    }
}

void MonsterTamer::monsterLvlChanged()
{
    int level = m_monster->getStats(Monster::LEVEL);

    m_ui->monsterLevel->display(level);
    m_save.setValue("level", level);
    m_ui->donjonLvl->setMaximum(level);
    m_ui->donjonLvlDial->setMaximum(level);
}
void MonsterTamer::monsterXpChanged()
{
    int xpCur = m_monster->getStats(Monster::XPCUR);
    int xpMax = m_monster->getStats(Monster::XPMAX);

    m_ui->monsterXPBar->setValue(xpCur);
    m_ui->monsterXPBar->setMaximum(xpMax);
    m_save.setValue("xpMin", xpCur);
    m_save.setValue("xpMax", xpMax);
}

void MonsterTamer::monsterHpChanged()
{
    int hpCur = m_monster->getStats(Monster::HPCUR);
    int hpMax = m_monster->getStats(Monster::HPMAX);

    m_ui->monsterHPBar->setValue(hpCur);
    m_ui->monsterHPBar->setMaximum(hpMax);
    m_save.setValue("hpMin", hpCur);
    m_save.setValue("hpMax", hpMax);
    if (m_monster->getAction() == Monster::WAITING && hpCur == hpMax)
        m_ui->restButton->setEnabled(false);
    else if (m_monster->getAction() == Monster::WAITING)
        m_ui->restButton->setEnabled(true);
}

void MonsterTamer::monsterHungerChanged()
{
    int hfpCur = m_monster->getStats(Monster::HUNGERCUR);
    int hfpMax = m_monster->getStats(Monster::HUNGERMAX);
    int food = m_monster->getStats(Monster::FOOD);
    int hunger = m_monster->getStats(Monster::HUNGERMAX) - m_monster->getStats(Monster::HUNGERCUR);

    m_ui->monsterHungerBar->setValue(hfpCur);
    m_ui->monsterHungerBar->setMaximum(hfpMax);
    m_ui->restButton->setEnabled(hfpCur == 0 ? false : m_ui->restButton->isEnabled());
    m_save.setValue("hungerMin", hfpCur);
    m_save.setValue("hungerMax", hfpMax);
    m_save.setValue("food", food);
    m_ui->tamerFood->display(food);
    m_ui->foodNb->setMaximum(hunger);
    m_ui->foodNbDial->setMaximum(hunger);
    if (hunger == 0 && m_monster->getAction() == Monster::WAITING) {
        m_ui->feedButton->setEnabled(false);
        m_ui->foodNb->setEnabled(false);
        m_ui->foodNbDial->setEnabled(false);
    }
    else if (m_monster->getAction() == Monster::WAITING){
        m_ui->feedButton->setEnabled(true);
        m_ui->foodNb->setEnabled(true);
        m_ui->foodNbDial->setEnabled(true);
    }
}

void MonsterTamer::monsterStatsToPlaceChanged()
{
    int statsToPlace = m_monster->getStats(Monster::TOPLACE);

    m_save.setValue("statsToPlace", statsToPlace);
    m_ui->monsterPoints->display(statsToPlace);
    if (statsToPlace == 0) {
        m_ui->addStr->setEnabled(false);
        m_ui->addEnd->setEnabled(false);
        m_ui->addSpd->setEnabled(false);
    }
}

void MonsterTamer::monsterStrChanged()
{
    int str = m_monster->getStats(Monster::STR);

    m_save.setValue("str", str);
    m_ui->monsterStr->display(str);
}

void MonsterTamer::monsterEndChanged()
{
    int end = m_monster->getStats(Monster::END);

    m_save.setValue("end", end);
    m_ui->monsterEnd->display(end);
}

void MonsterTamer::monsterSpdChanged()
{
    int spd = m_monster->getStats(Monster::SPD);

    m_save.setValue("spd", spd);
    m_ui->monsterSpd->display(spd);
}

void MonsterTamer::monsterFoodGained()
{
    int food = m_monster->getStats(Monster::FOOD);
    m_ui->tamerFood->display(food);
    m_save.setValue("food", food);
}

void MonsterTamer::monsterGoldGained()
{
    int gold = m_monster->getStats(Monster::GOLD);
    m_ui->tamerGold->display(gold);
    m_save.setValue("gold", gold);
}

void MonsterTamer::monsterActionChanged()
{
    m_ui->monsterAction->setText("Action: " + m_monster->getActionStr());
    if (m_monster->getAction() == Monster::WAITING) {
        m_ui->restButton->setEnabled(true);
        m_ui->feedButton->setEnabled(true);
        m_ui->foodNb->setEnabled(true);
        m_ui->foodNbDial->setEnabled(true);
        m_ui->trainButton->setEnabled(true);
        m_ui->trainingTime->setEnabled(true);
        m_ui->trainingTimeDial->setEnabled(true);
        m_ui->exploreButton->setEnabled(true);
        m_ui->exploreLvl->setEnabled(true);
        m_ui->exploreLvlDial->setEnabled(true);
        m_ui->donjonButton->setEnabled(true);
        m_ui->donjonLvl->setEnabled(true);
        m_ui->donjonLvlDial->setEnabled(true);
        m_ui->menuBar->setEnabled(true);
        m_ui->tabEquipements->setEnabled(true);
        if (m_monster->getStats(Monster::TOPLACE) > 0) {
            m_ui->addStr->setEnabled(true);
            m_ui->addEnd->setEnabled(true);
            m_ui->addSpd->setEnabled(true);
        }
    }
    else {
        m_ui->restButton->setEnabled(false);
        m_ui->feedButton->setEnabled(false);
        m_ui->foodNb->setEnabled(false);
        m_ui->foodNbDial->setEnabled(false);
        m_ui->trainButton->setEnabled(false);
        m_ui->trainingTime->setEnabled(false);
        m_ui->trainingTimeDial->setEnabled(false);
        m_ui->exploreButton->setEnabled(false);
        m_ui->exploreLvl->setEnabled(false);
        m_ui->exploreLvlDial->setEnabled(false);
        m_ui->donjonButton->setEnabled(false);
        m_ui->donjonLvl->setEnabled(false);
        m_ui->donjonLvlDial->setEnabled(false);
        m_ui->menuBar->setEnabled(false);
        m_ui->addStr->setEnabled(false);
        m_ui->addEnd->setEnabled(false);
        m_ui->addSpd->setEnabled(false);
        m_ui->tabEquipements->setEnabled(false);
    }
}

void MonsterTamer::monsterActionTimeChanged()
{
    int time = m_monster->getTime();
    int curTime = m_monster->getCurTime();

    if (time == 0) {
        m_ui->progressActionBar->setMaximum(100);
        m_ui->progressActionBar->setValue(100);
        m_ui->progressActionBar->setEnabled(false);
    }
    else {
        m_ui->progressActionBar->setMaximum(time);
        m_ui->progressActionBar->setValue(curTime);
        m_ui->progressActionBar->setEnabled(true);
    }
}

void MonsterTamer::monsterAliveChanged()
{
    if (!m_monster->isAlive()) {
        m_ui->monsterAction->setText("Action: DEAD!");
        m_save.clear();
        m_ui->restButton->setEnabled(false);
        m_ui->feedButton->setEnabled(false);
        m_ui->foodNb->setEnabled(false);
        m_ui->foodNbDial->setEnabled(false);
        m_ui->trainButton->setEnabled(false);
        m_ui->trainingTime->setEnabled(false);
        m_ui->trainingTimeDial->setEnabled(false);
        m_ui->exploreButton->setEnabled(false);
        m_ui->exploreLvl->setEnabled(false);
        m_ui->exploreLvlDial->setEnabled(false);
        m_ui->donjonButton->setEnabled(false);
        m_ui->donjonLvl->setEnabled(false);
        m_ui->donjonLvlDial->setEnabled(false);
        m_ui->addStr->setEnabled(false);
        m_ui->addEnd->setEnabled(false);
        m_ui->addSpd->setEnabled(false);
        m_ui->menuBar->setEnabled(true);
    }
}

void MonsterTamer::feedMonster()
{
    int food = m_monster->getStats(Monster::FOOD);
    int tmp = m_ui->foodNb->value();

    if (tmp > food)
        tmp = food;
    food -= tmp;
    m_monster->feed(tmp);
    m_ui->tamerFood->display(food);
}

void MonsterTamer::trainMonster()
{
    int tmp = (m_ui->trainingTime->time().hour() * 60 + m_ui->trainingTime->time().minute()) * 60;
    m_monster->train(tmp);
}

void MonsterTamer::exploreMonster()
{
    int curLvlExplo = m_ui->exploreLvl->value();
    int lvlMaxExplo;

    m_monster->explore(curLvlExplo);
    lvlMaxExplo = m_monster->getStats(Monster::LVLEXPLOMAX);
    m_ui->exploreLvl->setMaximum(lvlMaxExplo);
    m_ui->exploreLvlDial->setMaximum(lvlMaxExplo);
    m_save.setValue("lvlMaxExplore", lvlMaxExplo);
}

void MonsterTamer::donjonMonster()
{
    int curLvlDonjon = m_ui->donjonLvl->value();
    m_monster->donjon(curLvlDonjon);
}

void MonsterTamer::saveMonster()
{
    QString filePath = QFileDialog::getSaveFileName(m_ui->centralwidget, "Save Monster",
                                                    "", "monster files (*.mf)");
    QSettings settings(filePath, QSettings::IniFormat);

    if (!filePath.isEmpty()) {
        settings.setValue("Name", m_monster->getName());
        settings.setValue("Type", m_monster->getTypeInt());
        settings.setValue("Img", m_save.value("Img"));
        settings.setValue("food", m_monster->getStats(Monster::FOOD));
        settings.setValue("gold", m_monster->getStats(Monster::GOLD));
        settings.setValue("lvlMaxExplore", m_monster->getStats(Monster::LVLEXPLOMAX));
        settings.setValue("level", m_monster->getStats(Monster::LEVEL));
        settings.setValue("xpMin", m_monster->getStats(Monster::XPCUR));
        settings.setValue("xpMax", m_monster->getStats(Monster::XPMAX));
        settings.setValue("hpMin", m_monster->getStats(Monster::HPCUR));
        settings.setValue("hpMax", m_monster->getStats(Monster::HPMAX));
        settings.setValue("hungerMin", m_monster->getStats(Monster::HUNGERCUR));
        settings.setValue("hungerMax", m_monster->getStats(Monster::HUNGERMAX));
        settings.setValue("statsToPlace", m_monster->getStats(Monster::TOPLACE));
        settings.setValue("str", m_monster->getStats(Monster::STR));
        settings.setValue("end", m_monster->getStats(Monster::END));
        settings.setValue("spd", m_monster->getStats(Monster::SPD));
        saveMonsterInventory(&settings);
    }
}

void MonsterTamer::loadMonster()
{
    QString filePath = QFileDialog::getOpenFileName(m_ui->centralwidget, "Load Monster",
                                                    "", "monster files (*.mf)");
    QSettings settings(filePath, QSettings::IniFormat);

    if (!filePath.isEmpty()) {
        m_save.setValue("Name", settings.value("Name"));
        m_save.setValue("Type", settings.value("Type"));
        m_save.setValue("Img", settings.value("Img"));
        m_save.setValue("food", settings.value("food"));
        m_save.setValue("gold", settings.value("gold"));
        m_save.setValue("lvlMaxExplore", settings.value("lvlMaxExplore"));
        m_save.setValue("level", settings.value("level"));
        m_save.setValue("xpMin", settings.value("xpMin"));
        m_save.setValue("xpMax", settings.value("xpMax"));
        m_save.setValue("hpMin", settings.value("hpMin"));
        m_save.setValue("hpMax", settings.value("hpMax"));
        m_save.setValue("hungerMin", settings.value("hungerMin"));
        m_save.setValue("hungerMax", settings.value("hungerMax"));
        m_save.setValue("statsToPlace", settings.value("StatsToPlace"));
        m_save.setValue("str", settings.value("str"));
        m_save.setValue("end", settings.value("end"));
        m_save.setValue("spd", settings.value("spd"));
        init();
        m_monster->update();
        m_ui->progressActionBar->setValue(0);
        m_monster->clearEquipement();
        loadMonsterInventory(&settings);
    }
}

void MonsterTamer::resetMonster()
{
    if (QMessageBox::question(m_ui->centralwidget, "Reset Monster",
                          "Are you sure you want to reset your monster ?",
                          QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
        m_save.clear();
        init();
        m_monster->update();
    }
}

void MonsterTamer::setFoodNb(const int value)
{
    m_ui->foodNb->setValue(value);
}

void MonsterTamer::setTrainTime(const int value)
{
    QTime time;
    int h = value / 3600;
    int tmp = value - (h * 3600);
    int m =  tmp / 60;
    int s = tmp % 60;

    if (time.setHMS(h,m, s,0))
        m_ui->trainingTime->setTime(time);
}

void MonsterTamer::setExploreLevel(const int value)
{
    m_ui->exploreLvl->setValue(value);
}

void MonsterTamer::setDonjonLevel(const int value)
{
    m_ui->donjonLvl->setValue(value);
}

void MonsterTamer::displayItems()
{
    std::shared_ptr<Equipement> eq;

    for (int i = 0; i < m_inventory.size(); i++) {
        if (i < 4) {
            eq = m_monster->getEquipement(true, i);
            if (i < 2){
                if (eq == NULL)
                    m_inventory[i]->setStyleSheet(getItemStyleSheet(0, 0));
                else
                    m_inventory[i]->setStyleSheet(getItemStyleSheet(eq->getRarityInt(), 0));
            }
            else {
                if (eq == NULL)
                    m_inventory[i]->setStyleSheet(getItemStyleSheet(0, 1));
                else
                    m_inventory[i]->setStyleSheet(getItemStyleSheet(eq->getRarityInt(), 1));
            }
        }
        else {
            eq = m_monster->getEquipement(false, i - 4);
            if (eq == NULL)
                m_inventory[i]->setStyleSheet(getItemStyleSheet(0, 2));
            else
                m_inventory[i]->setStyleSheet(getItemStyleSheet(eq->getRarityInt(), 2));
        }
        if (eq != NULL)
            m_inventory[i]->setPixmap(getItemImg(eq->getType()));
        else
            m_inventory[i]->clear();
    }
    m_ui->itemStrBonus->display(m_monster->getStats(Monster::STRBOOST));
    m_ui->itemEndBonus->display(m_monster->getStats(Monster::ENDBOOST));
    m_ui->itemSpdBonus->display(m_monster->getStats(Monster::SPDBOOST));
    saveMonsterInventory(&m_save);
}

QString MonsterTamer::getItemStyleSheet(const int backGround, const int border)
{
    QString ssBackGround[5] = {"background-color: rgb(255, 255, 255);",
                               "background-color: rgb(160, 160, 160);",
                               "background-color: rgb(80, 200, 0);",
                               "background-color: rgb(0, 200, 200);",
                               "background-color: rgb(255, 215, 0);"};
    QString ssBorder[3] = {" border: 2px solid red;",
                           " border: 2px solid blue;",
                           " border: 2px solid grey;"};
    return QString(ssBackGround[backGround] + ssBorder[border] + " border-radius: 5px;");
}

QString MonsterTamer::getItemImg(const int type)
{
    QString img[4] = {"img/Stuff/Claws.png",
                      "img/Stuff/Fangs.png",
                      "img/Stuff/HeadArmor.png",
                      "img/Stuff/ChestArmor.png"};
    return img[type];
}

void MonsterTamer::displayItemStats(const MyLabel *place)
{
    std::shared_ptr<Equipement> item = m_monster->getEquipement(place->isEquiped(), place->getPos());

    qDebug() << "Clicked";
    if (item) {
        m_ui->labelNameItem->setText(item->getRarityStr() + " " + item->getTypeStr());
        m_ui->itemStr->display(item->getStats(Equipement::STR));
        m_ui->itemEnd->display(item->getStats(Equipement::END));
        m_ui->itemSpeed->display(item->getStats(Equipement::SPD));

        if (!place->isEquiped()) {
            m_ui->sellButton->setEnabled(true);
            m_monster->setItemToSell(place->getPos());
        }
        else
            m_ui->sellButton->setEnabled(false);
    }
}

void MonsterTamer::sellItem()
{
    m_monster->sellItem();
    m_ui->sellButton->setEnabled(false);
    m_ui->tamerGold->display(m_monster->getStats(Monster::GOLD));
}
