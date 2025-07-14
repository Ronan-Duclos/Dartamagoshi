#include "EquipementFabricator.hpp"

#include <QRandomGenerator>

EquipementFabricator &EquipementFabricator::Instance()
{
    static EquipementFabricator instance;
    return instance;
}

std::shared_ptr<Equipement> EquipementFabricator::getEquipement(const Equipement::e_rarity rarity, const Equipement::e_type type, const int stats[])
{
    std::shared_ptr<Equipement> ret(new Equipement(rarity, type, stats));
    return ret;
}

std::shared_ptr<Equipement> EquipementFabricator::getNewEquipement(const int level)
{
    std::vector<int> tab = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                            1, 1, 2, 1, 1, 1, 1, 1, 1, 1,
                            1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                            1, 1, 1, 1, 2, 1, 1, 1, 1, 1,
                            1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                            1, 3, 1, 1, 1, 1, 1, 1, 1, 1,
                            1, 1, 1, 1, 1, 1, 2, 1, 1, 1,
                            1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                            1, 1, 1, 4, 1, 1, 1, 1, 1, 1,
                            1, 1, 1, 1, 1, 1, 1, 1, 3, 1};

    Equipement::e_rarity rarity = static_cast<Equipement::e_rarity>(tab[(QRandomGenerator::global()->generate() % 100)]);
    Equipement::e_type type = static_cast<Equipement::e_type>(QRandomGenerator::global()->generate() % 4);
    int my_stats[4];
    my_stats[Equipement::LEVEL] = level;
    double multiple;

    switch (rarity) {
    case 1:
        multiple = 1;
        break;
    case 2:
        multiple = 1.5;
        break;
    case 3:
        multiple = 2;
        break;
    case 4:
        multiple = 2.5;
        break;
    default:
        break;
    }
    my_stats[Equipement::STR] = ((my_stats[Equipement::LEVEL] / 10) + (QRandomGenerator::global()->generate() % 3)) * multiple;
    my_stats[Equipement::END] = ((my_stats[Equipement::LEVEL] / 10) + (QRandomGenerator::global()->generate() % 3)) * multiple;
    my_stats[Equipement::SPD] = ((my_stats[Equipement::LEVEL] / 10) + (QRandomGenerator::global()->generate() % 3)) * multiple;
    if (my_stats[Equipement::STR] == 0 && my_stats[Equipement::END] == 0 && my_stats[Equipement::SPD] == 0) {
        int tmp = QRandomGenerator::global()->generate() % 3;
        switch (tmp) {
        case 0:
            my_stats[Equipement::STR] = 1;
            break;
        case 1:
            my_stats[Equipement::END] = 1;
            break;
        case 2:
            my_stats[Equipement::SPD] = 1;
            break;
        default:
            break;
        }
    }
    std::shared_ptr<Equipement> ret(new Equipement(rarity, type, my_stats));
    return ret;
}

EquipementFabricator::EquipementFabricator()
{

}

