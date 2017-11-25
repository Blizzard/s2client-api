#pragma once

#include "sc2_proto_interface.h"
#include "sc2_gametypes.h"
#include "sc2_typeenums.h"
#include <vector>
#include <string>
#include <stdint.h>

namespace sc2 {

class ObservationInterface;

typedef MessageResponsePtr<SC2APIProtocol::ResponseData> ResponseDataPtr;

//! Indicates if an ability is available, and if that ability requires a point.
struct AvailableAbility {
    AvailableAbility() = default;
    AvailableAbility(AbilityID ability_id, bool requires_point) :
        ability_id(ability_id),
        requires_point(requires_point) {};

    //! Ability that is available.
    AbilityID ability_id = 0;
    //! Indicates if the ability requires a point to invoke.
    bool requires_point = false;
};

//! Data about an ability.
struct AbilityData {
    //! Type of target this ability applies to.
    enum class Target {
        //! There is no target, applies to self.
        None = 1,
        //! Target is a point.
        Point = 2,
        //! Target is another unit.
        Unit = 3,
        //! Target can be a point or another unit.
        PointOrUnit = 4,
        //! Target can be a point or no target.
        PointOrNone = 5
    };

    //! If true, the ability is available to the current mods/map.
    bool available;
    //! Stable ID. For a given ability, this value will be immutable throughout different patches.
    AbilityID ability_id;
    //! Catalog (game data xml) name of the ability.
    std::string link_name;
    //! Catalog (game data xml) index of the ability.
    uint32_t link_index;
    //! Name of the button for the command card.
    std::string button_name;
    //! In case the button name is not descriptive.
    std::string friendly_name;
    //! UI hotkey.
    std::string hotkey;
    //! This ability id may be represented by the given more generic id.
    uint32_t remaps_to_ability_id;
    //! Other abilities remapped to this generic ability.
    std::vector<uint32_t> remaps_from_ability_id;
    //! Determines if a point, unit, both or none is required as a target.
    Target target;
    //! Can be cast in the minimap. (Not implemented)
    bool allow_minimap;
    //! Autocast can be set.
    bool allow_autocast;
    //! Requires placement to construct a building.
    bool is_building;
    //! If the ability is placing a building, give the radius of the footprint.
    float footprint_radius;
    //! Placement next to an existing structure, e.g., an add-on like a Tech Lab.
    bool is_instant_placement;
    //! Range unit can cast ability without needing to approach target.
    float cast_range;

    //! AbilityData constructor.
    AbilityData();

    //! Serialize this ability entry from the .proto file (used internally).
    //!< \param ability_data The proto entry for this ability.
    void ReadFromProto(const SC2APIProtocol::AbilityData& ability_data);
    //! Serialize this ability to a string.
    std::string Log() const;
};

typedef std::vector<AbilityData> Abilities;

//! All available abilities for a unit.
struct AvailableAbilities {
    //! The available abilities.
    std::vector<AvailableAbility> abilities;
    //! The unit.
    Tag unit_tag;
    //! The unit type.
    UnitTypeID unit_type_id;

    //! Returns true if this object refers to a valid unit and unit type.
    //!< \return If this object is valid.
    bool IsValid() const {
        return unit_tag && unit_type_id;
    }
};

//! Category of unit.
enum class Attribute {
    Light = 1,
    Armored = 2,
    Biological = 3,
    Mechanical = 4,
    Robotic = 5,
    Psionic = 6,
    Massive = 7,
    Structure = 8,
    Hover = 9,
    Heroic = 10,
    Summoned = 11,
    Invalid = 12
};

//! Damage bonus of unit.
struct DamageBonus {
    Attribute attribute;
    float bonus;

    DamageBonus();

    void ReadFromProto(const SC2APIProtocol::DamageBonus& damage_bonus);
};

//! Unit weapon.
struct Weapon {
    enum class TargetType {
        Ground = 1,
        Air = 2,
        Any = 3,
        Invalid = 4
    };
    TargetType type;
    float damage_;
    std::vector<DamageBonus> damage_bonus;              // Extra damage when attacking a unit of a certain attribute
    uint32_t attacks;                                   // Number of hits per attack. (eg. Colossus has 2 beams)
    float range;
    float speed;                                        // Time between attacks

    Weapon();

    void ReadFromProto(const SC2APIProtocol::Weapon& weapon);
};

//! Data about a unit type. This data is derived from the catalog (xml) data of the game and upgrades.
struct UnitTypeData {
    //! Stable ID. This ID will not change between patches.
    UnitTypeID unit_type_id;
    //! Unit type name, corresponds to the game's catalog.
    std::string name;
    //! If true, the unit is available to the current mods/map.
    bool available;
    //! Number of cargo slots they occupy in a transport.
    uint32_t cargo_size;
    //! Cost in minerals to build this unit type.
    int mineral_cost;
    //! Cost in vespene to build this unit type.
    int vespene_cost;

    //! Unit attributes, may change based on upgrades.
    std::vector<Attribute> attributes;
    //! Movement speed of unit type.
    float movement_speed;
    //! Armor of unit type.
    float armor;
    //! Weapons on this unit type.
    std::vector<Weapon> weapons;
    //! How much food the unit requires.
    float food_required;
    //! How much food the unit provides.
    float food_provided;
    //! Which ability id creates the unit.
    AbilityID ability_id;
    //! The race the unit belongs to.
    Race race;
    //! How long the unit takes to build.
    float build_time;
    //! Whether the unit can have minerals (mineral patches).
    bool has_minerals;
    //! Whether the unit can have vespene (vespene geysers).
    bool has_vespene;
    //! Range the unit reveals vision.
    float sight_range;

    //! Units this is equivalent to in terms of satisfying tech requirements.
    std::vector<UnitTypeID> tech_alias;
    //! Units that are morphed variants of the same unit.
    UnitTypeID unit_alias;
    //! Structure required to build this unit. (Or any with the same tech_alias)
    UnitTypeID tech_requirement;
    //! Whether tech_requirement is an add-on.
    bool require_attached;

    //! Constructor.
    UnitTypeData();

    //! Serialize this ability entry from the .proto file (used internally).
    //!< \param unit_data The proto entry for this ability.
    void ReadFromProto(const SC2APIProtocol::UnitTypeData& unit_data);
    //! Serialize this unit type to a string.
    std::string Log() const;
};

typedef std::vector<UnitTypeData> UnitTypes;

//! Upgrade data.
struct UpgradeData {
    //! Stable ID. This ID will not change between patches.
    uint32_t upgrade_id;
    //! Upgrade name, corresponds to the game's catalog.
    std::string name;
    //! Mineral cost of researching the upgrade.
    uint32_t mineral_cost;
    //! Vespene cost of researching the upgrade.
    uint32_t vespene_cost;
    //! Ability that researches this upgrade.
    AbilityID ability_id;
    //! Time in GameLoops to research this upgrade.
    float research_time;

    UpgradeData();

    void ReadFromProto(const SC2APIProtocol::UpgradeData& upgrade_data);
    std::string Log() const;
};

typedef std::vector<UpgradeData> Upgrades;

//! Buff data.
struct BuffData {
    //! Stable ID. This ID will not change between patches.
    uint32_t buff_id;
    //! Buff name, corresponds to the game's catalog.
    std::string name;

    BuffData();

    void ReadFromProto(const SC2APIProtocol::BuffData& buff_data);
    std::string Log() const;
};

typedef std::vector<BuffData> Buffs;

//! Effect data.
struct EffectData {
    //! Stable ID. This ID will not change between patches.
    uint32_t effect_id;
    //! Effect name, corresponds to the game's catalog.
    std::string name;
    //! A more recognizable name of the effect.
    std::string friendly_name;
    //! Size of the circle the effect impacts.
    float radius;

    void ReadFromProto(const SC2APIProtocol::EffectData& effect_data);
    std::string Log() const;
};

typedef std::vector<EffectData> Effects;

//! Power source information for Protoss.
struct PowerSource {
    PowerSource(const Point2D in_position, float in_radius, Tag in_tag) :
        position(in_position),
        radius(in_radius),
        tag(in_tag) {};

    //! Power source position.
    Point2D position;
    //! Power source radius.
    float radius;
    //! Unit tag of the power source.
    Tag tag;
};

//! The visuals of a persistent ability on the map. (eg. Psistorm)
struct Effect {
    //! Type of the effect
    uint32_t effect_id;
    //! All the positions that this effect is impacting on the map.
    //! eg. The Lurker's attack impacts multiple positions in a line.
    std::vector<Point2D> positions;

    void ReadFromProto(const SC2APIProtocol::Effect& effect);
};

AbilityID GetGeneralizedAbilityID(uint32_t ability_id, const ObservationInterface& observation);

}
