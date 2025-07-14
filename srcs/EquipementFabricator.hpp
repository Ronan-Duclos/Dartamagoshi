#pragma once

#include "Equipement.hpp"

class EquipementFabricator
{
public:
    static EquipementFabricator &Instance();
    std::shared_ptr<Equipement> getEquipement(const Equipement::e_rarity rarity, const Equipement::e_type type, const int stats[4]);
    std::shared_ptr<Equipement> getNewEquipement(const int level);

private:
    EquipementFabricator();
    EquipementFabricator(EquipementFabricator const&);
    void operator=(EquipementFabricator const&);
    //static EquipementFabricator m_singleton;
};

