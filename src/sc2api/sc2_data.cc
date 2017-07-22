#include "sc2api/sc2_data.h"
#include "sc2api/sc2_interfaces.h"

#include <iostream>
#include <cassert>

#include "sc2api.pb.h"

// TODO: Fill out the log functions

namespace sc2 {

AbilityData::AbilityData() :
    available(false),
    ability_id(0),
    link_index(0),
    remaps_to_ability_id(0),
    target(Target::None),
    allow_minimap(false),
    allow_autocast(false),
    is_building(false),
    footprint_radius(0.0f),
    is_instant_placement(false),
    cast_range(0.0f) {
}

void AbilityData::ReadFromProto(const SC2APIProtocol::AbilityData& ability_data) {
    // ability_id_
    if (ability_data.has_ability_id()) {
        ability_id = ability_data.ability_id();
    }

    // available_
    available = false;
    if (ability_data.has_available()) {
        available = ability_data.available();
    }

    // link_name_
    if (ability_data.has_link_name()) {
        link_name = ability_data.link_name();
    }

    // link_index_
    link_index = 0;
    if (ability_data.has_link_index()) {
        link_index = ability_data.link_index();
    }

    // button_name_
    if (ability_data.has_button_name()) {
        button_name = ability_data.button_name();
    }

    // friendly_name_
    if (ability_data.has_friendly_name()) {
        friendly_name = ability_data.friendly_name();
    }

    // hotkey_
    if (ability_data.has_hotkey()) {
        hotkey = ability_data.hotkey();
    }

    // Remaps.
    remaps_to_ability_id = 0;
    if (ability_data.has_remaps_to_ability_id()) {
        remaps_to_ability_id = ability_data.remaps_to_ability_id();
    }

    // target_
    if (ability_data.has_target()) {
        switch (ability_data.target()) {
            case SC2APIProtocol::AbilityData_Target_Point: {
                target = Target::Point;
                break;
            }
            case SC2APIProtocol::AbilityData_Target_Unit: {
                target = Target::Unit;
                break;
            }
            case SC2APIProtocol::AbilityData_Target_PointOrUnit: {
                target = Target::PointOrUnit;
                break;
            }
            case SC2APIProtocol::AbilityData_Target_PointOrNone: {
                target = Target::PointOrNone;
                break;
            }
            case SC2APIProtocol::AbilityData_Target_None: {
            }
            default: {
                target = Target::None;
                break;
            }
        }
    }

    // allow_minimap_
    allow_minimap = 0;
    if (ability_data.has_allow_minimap()) {
        allow_minimap = ability_data.allow_minimap();
    }

    // allow_minimap_
    allow_autocast = 0;
    if (ability_data.has_allow_autocast()) {
        allow_autocast = ability_data.allow_autocast();
    }

    // is_building_
    is_building = false;
    if (ability_data.has_is_building()) {
        is_building = ability_data.is_building();
    }

    // footprint_radius_
    footprint_radius = 0.0f;
    if (ability_data.has_footprint_radius()) {
        footprint_radius = ability_data.footprint_radius();
    }

    is_instant_placement = false;
    if (ability_data.has_is_instant_placement()) {
        is_instant_placement = ability_data.has_is_instant_placement();
    }

    cast_range = 0.0f;
    if (ability_data.has_cast_range()) {
        cast_range = ability_data.cast_range();
    }
}

std::string AbilityData::Log() const {
    std::string str_out;
    str_out = ability_id.to_string() + ":\n";
    if (button_name.length() > 0) {
        str_out += "  Button: " + button_name + "\n";
    }
    if (hotkey.length() > 0) {
        str_out += "  Hotkey: " + hotkey + "\n";
    }
    str_out += "  " + (link_name.length() > 0 ? link_name : "Null") + ", " + std::to_string(link_index) + "\n";
    if (is_building) {
        str_out += "  Building footprint: " + std::to_string(footprint_radius) + "\n";
    }

    switch (target) {
        case Target::Point: {
            str_out += "  Target: Point\n";
            break;
        }
        case Target::Unit: {
            str_out += "  Target: Unit\n";
            break;
        }
        case Target::PointOrUnit: {
            str_out += "  Target: Point or unit\n";
            break;
        }
        case Target::None: {
        }
        default: {
            str_out += "  Target: None\n";
            break;
        }
    }

    str_out += "  Autocast: " + std::string(allow_autocast ? "Yes" : "No") + "\n";
    str_out += "  Minimap: " + std::string(allow_minimap ? "Yes" : "No") + "\n";

    return str_out;
}

static Attribute ConvertAttributeEnum(SC2APIProtocol::Attribute attribute) {
    switch (attribute) {
        case SC2APIProtocol::Attribute::Light:       return Attribute::Light;
        case SC2APIProtocol::Attribute::Armored:     return Attribute::Armored;
        case SC2APIProtocol::Attribute::Biological:  return Attribute::Biological;
        case SC2APIProtocol::Attribute::Mechanical:  return Attribute::Mechanical;
        case SC2APIProtocol::Attribute::Robotic:     return Attribute::Robotic;
        case SC2APIProtocol::Attribute::Psionic:     return Attribute::Psionic;
        case SC2APIProtocol::Attribute::Massive:     return Attribute::Massive;
        case SC2APIProtocol::Attribute::Structure:   return Attribute::Structure;
        case SC2APIProtocol::Attribute::Hover:       return Attribute::Hover;
        case SC2APIProtocol::Attribute::Heroic:      return Attribute::Heroic;
        case SC2APIProtocol::Attribute::Summoned:    return Attribute::Summoned;
        default:                                        return Attribute::Invalid;
    }
}

DamageBonus::DamageBonus() {
}

void DamageBonus::ReadFromProto(const SC2APIProtocol::DamageBonus& damage_bonus) {
    // attribute_
    attribute = ConvertAttributeEnum(damage_bonus.attribute());

    // bonus_
    bonus = damage_bonus.bonus();
}

static Weapon::TargetType ConvertTargetTypeEnum(SC2APIProtocol::Weapon::TargetType type) {
    switch (type) {
        case SC2APIProtocol::Weapon::Ground:     return Weapon::TargetType::Ground;
        case SC2APIProtocol::Weapon::Air:        return Weapon::TargetType::Air;
        case SC2APIProtocol::Weapon::Any:        return Weapon::TargetType::Any;
        default:                                    return Weapon::TargetType::Invalid;
    }
}

Weapon::Weapon() {
}

void Weapon::ReadFromProto(const SC2APIProtocol::Weapon& weapon) {
    // type_
    type = ConvertTargetTypeEnum(weapon.type());

    // damage_
    damage_ = weapon.damage();

    // damage_bonus_
    for (int i = 0; i < weapon.damage_bonus_size(); ++i) {
        DamageBonus bonus;
        bonus.ReadFromProto(weapon.damage_bonus(i));
        damage_bonus.push_back(bonus);
    }

    // attacks_
    attacks = weapon.attacks();

    // range_
    range = weapon.range();

    // speed_
    speed = weapon.speed();
}

UnitTypeData::UnitTypeData() {
}

void UnitTypeData::ReadFromProto(const SC2APIProtocol::UnitTypeData& unit_data) {
    // unit_type_id_
    unit_type_id = unit_data.unit_id();

    // name_
    name = unit_data.name();

    // available_
    available = unit_data.available();

    // cargo_size_
    cargo_size = unit_data.cargo_size();

    // mineral_cost_
    mineral_cost = unit_data.mineral_cost();

    // vespene_cost_
    vespene_cost = unit_data.vespene_cost();

    // attribute_
    for (int i = 0; i < unit_data.attributes_size(); ++i) {
        attributes.push_back(ConvertAttributeEnum(unit_data.attributes(i)));
    }

    // movement_speed_
    movement_speed = unit_data.movement_speed();

    // armor_
    armor = unit_data.armor();

    // weapons_
    for (int i = 0; i < unit_data.weapons_size(); ++i) {
        Weapon weapon;
        weapon.ReadFromProto(unit_data.weapons(i));
        weapons.push_back(weapon);
    }
}

std::string UnitTypeData::Log() const {
    std::string str_out;

    str_out = unit_type_id.to_string() + ":\n";
    str_out += "  " + (name.length() > 0 ? name : "Null") + "\n";

    return str_out;
}

UpgradeData::UpgradeData() {
}

void UpgradeData::ReadFromProto(const SC2APIProtocol::UpgradeData& upgrade_data) {
    // upgrade_id_
    upgrade_id = upgrade_data.upgrade_id();

    // name_
    name = upgrade_data.name();
}

std::string UpgradeData::Log() const {
    std::string str_out;

    return str_out;
}

BuffData::BuffData() {
}

void BuffData::ReadFromProto(const SC2APIProtocol::BuffData& buff_data) {
    // upgrade_id_
    buff_id = buff_data.buff_id();

    // name_
    name = buff_data.name();
}

std::string BuffData::Log() const {
    std::string str_out;

    return str_out;
}

AbilityID GetGeneralizedAbilityID(uint32_t ability_id, const ObservationInterface& observation) {
    if (ability_id == 0) {
        return AbilityID(ability_id);
    }

    const Abilities& abilities = observation.GetAbilityData();
    if (ability_id >= abilities.size()) {
        assert(0);
        return AbilityID(ability_id);
    }

    const AbilityData& ability = abilities[ability_id];
    if (ability.remaps_to_ability_id != 0) {
        return AbilityID(ability.remaps_to_ability_id);
    }

    return AbilityID(ability_id);
}

}
