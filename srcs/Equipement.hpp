#pragma once

#include <QWidget>

class Equipement
{
public:
    enum    e_rarity{
        COMMUN = 1,
        RARE = 2,
        EPIC = 3,
        LEGENDARY = 4
    };

    enum    e_type {
        CLAWS,
        FANGS,
        HEADARMOR,
        CHESTARMOR
    };

    enum    e_stats {
        LEVEL,
        STR,
        END,
        SPD
    };

    Equipement(const e_rarity rarity, const e_type type, const int stats[4]);
    Equipement(const Equipement &copy);

    Equipement& operator=(const Equipement &ass);

    e_rarity getRarity();
    QString getRarityStr();
    int getRarityInt();
    e_type getType();
    QString getTypeStr();
    int getTypeInt();
    int getStats(e_stats stats);

private:
    e_rarity m_rarity;
    e_type m_type;
    int m_stats[4];
};
