#pragma once

#include "Equipement.hpp"

class EquipementFabricator
{
public:
    static EquipementFabricator &Instance();
    std::shared_ptr<Equipement> getEquipement(Equipement::e_rarity rarity, Equipement::e_type type, int stats[4]);
    std::shared_ptr<Equipement> getNewEquipement(int level);

private:
    EquipementFabricator();
    EquipementFabricator(EquipementFabricator const&);
    void operator=(EquipementFabricator const&);
    //static EquipementFabricator m_singleton;
};

