#pragma once

#include <QMainWindow>
#include "srcs/MonsterTamer.hpp"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void updateTamer();

private:
    Ui::MainWindow  *ui;
    std::unique_ptr<MonsterTamer>    m_tamer;
};
