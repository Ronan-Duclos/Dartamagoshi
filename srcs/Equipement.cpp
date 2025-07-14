#include "Equipement.hpp"

#include <QRandomGenerator>

Equipement::Equipement(const e_rarity rarity, const e_type type, const int stats[4]) :
    m_stats{stats[0], stats[1], stats[2], stats[3]}, m_rarity(rarity), m_type(type)
{

}

Equipement::Equipement(const Equipement &copy)
{
    *this = copy;
}

Equipement &Equipement::operator=(const Equipement &ass)
{
    m_rarity = ass.m_rarity;
    m_type = ass.m_type;
    for (int i = 0; i < 4; i++)
        m_stats[i] = ass.m_stats[i];
    return *this;
}

Equipement::e_rarity Equipement::getRarity()
{
    return m_rarity;
}

QString Equipement::getRarityStr()
{
    std::vector<QString> ret = {"", "Commun", "Rare", "Epic", "Legendary"};
    return ret[m_rarity];
}

int Equipement::getRarityInt()
{
    return static_cast<int>(m_rarity);
}

Equipement::e_type Equipement::getType()
{
    return m_type;
}

QString Equipement::getTypeStr()
{
    std::vector<QString> ret = {"Claws", "Fangs", "Head Armor", "Chest Armor"};
    return ret[m_type];
}

int Equipement::getTypeInt()
{
    return m_type;
}

int Equipement::getStats(const e_stats stats)
{
    return m_stats[stats];
}
