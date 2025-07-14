#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QLayout>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_tamer(new MonsterTamer())
{
    ui->setupUi(this);
    m_tamer->setMonster(ui);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateTamer()
{
    m_tamer->upDateMonsterInfo();
}

