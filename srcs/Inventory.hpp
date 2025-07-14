#ifndef INVENTORY_HPP
#define INVENTORY_HPP

#include <QObject>

#include "Equipement.hpp"

class Inventory : public QObject
{
    Q_OBJECT
public:
    explicit Inventory(QObject *parent = nullptr);
    ~Inventory();

    Equipement *getEquipement(bool equiped, int pos);
    void addEquipement();

signals:

private:
    QList<Equipement *>  m_wearing;
    QList<Equipement *>  m_inventory;

};

#endif // INVENTORY_HPP
