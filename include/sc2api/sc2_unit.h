/*! \file sc2_unit.h
    \brief Unit data in an observation.
*/
#pragma once

#include "sc2_proto_interface.h"
#include "sc2_gametypes.h"
#include "sc2_common.h"
#include "sc2_typeenums.h"
#include <vector>
#include <unordered_map>
#include <string>
#include <stdint.h>

namespace sc2 {

class ObservationInterface;

//! An order that is active on a unit.
struct UnitOrder {
    //! Ability ID that triggered the order.
    AbilityID ability_id;
    //! Target unit of the order, if there is one.
    Tag target_unit_tag;
    //! Target position of the order, if there is one.
    Point2D target_pos;
    //! Progress of the order.
    float progress;

    UnitOrder() :
        ability_id(0),
        target_unit_tag(NullTag),
        progress(0.0f) {
    }
};

//! A passenger on a transport.
struct PassengerUnit {
    //! The tag of the unit in the transport.
    Tag tag;
    //! The health of the unit in the transport.
    float health;
    //! The max possible health of the unit in the transport.
    float health_max;
    //! The shield of the unit in the transport.
    float shield;
    //! The energy of the unit in the transport.
    float energy;
    //! The type of unit in the transport.
    UnitTypeID unit_type;

    PassengerUnit() :
        tag(NullTag),
        health(0.0f),
        health_max(0.0f),
        shield(0.0f),
        energy(0.0f),
        unit_type(0) {
    }
};

//! A unit. Could be a structure, a worker or a military unit.
class Unit {
public:
    //! If the unit is shown on screen or not.
    enum DisplayType {
        //! Unit will be visible.
        Visible = 1,
        //! Unit is represented by a snapshot in the fog-of-war. This is for units that don't belong to the player.
        //! The actual unit may be in a different location or state.
        Snapshot = 2,
        //! Unit will be hidden to enemies.
        Hidden = 3,
    };

    //! Relationship to this player.
    enum Alliance {
        //! Belongs to the player.
        Self = 1,
        //! Ally of the player.
        Ally = 2,
        //! A neutral unit, usually a non-player unit like a mineral field.
        Neutral = 3,
        //! Enemy of the player.
        Enemy = 4
    };

    //! Unit cloak state.
    enum CloakState {
        //! Cloaked, invisible to enemies until detected.
        Cloaked = 1,
        //! Cloaked enemy, but detected.
        CloakedDetected = 2,
        //! No cloaking.
        NotCloaked = 3,
        //! Could not determine cloaking state.
        Unknown = 4
    };

    //! If the unit is shown on screen or not.
    DisplayType display_type;
    //! Relationship of the unit to this player.
    Alliance alliance;

    //! A unique identifier for the instance of a unit.
    Tag tag;
    //! An identifier of the type of unit.
    UnitTypeID unit_type;
    //! Which player owns a unit.
    int owner;

    //! Position of the unit in the world.
    Point3D pos;
    //! Direction the unit faces in radians (1 radian == 57.296 degrees)
    float facing;
    //! Radius of the unit.
    float radius;
    //! Gives progress under construction. Range: [0.0, 1.0]. 1.0 == finished.
    float build_progress;

    //! If the unit is cloaked.
    CloakState cloak;

    //! Range of detector for detector units.
    float detect_range;
    //! Range of radar for units that are radar units.
    float radar_range;

    //! If the unit is in the current selection of the player.
    bool is_selected;
    //! Visible and within the camera frustum.
    bool is_on_screen;
    //! Detected by sensor tower.
    bool is_blip;

    // Not populated for snapshots

    //! Health of the unit. Not set for snapshots.
    float health;
    //! Max health for the unit. Not set for snapshots.
    float health_max;
    //! Shield of the unit. Not set for snapshots.
    float shield;
    //! Energy of the unit. Not set for snapshots.
    float energy;
    //! Amount of minerals if the unit is a mineral field. Not set for snapshots.
    int mineral_contents;
    //! Amount of vespene if the unit is a geyser. Not set for snapshots.
    int vespene_contents;
    //! If the unit is flying. Not set for snapshots.
    bool is_flying;
    //! If the unit is burrowed. Not set for snapshots.
    bool is_burrowed;
    //! Time remaining for a weapon on cooldown. Not set for snapshots.
    float weapon_cooldown;

    // Not populated for enemies/snapshots

    //! Orders on a unit. Only valid for this player's units.
    std::vector<UnitOrder> orders;
    //! Add-on like a tech lab or reactor. Only valid for this player's units.
    Tag add_on_tag;
    //! Passengers in this transport. Only valid for this player's units.
    std::vector<PassengerUnit> passengers;
    //! Number of cargo slots used in the transport. Only valid for this player's units.
    int cargo_space_taken;
    //! Number of cargo slots available for a transport. Only valid for this player's units.
    int cargo_space_max;
    //! Number of harvesters associated with a town hall (e.g., Command Center). Only valid for this player's units.
    int assigned_harvesters;
    //! Number of harvesters that can be assigned to a town hall (e.g., Command Center). Only valid for this player's units.
    int ideal_harvesters;
    //! Target unit of a unit. Only valid for this player's units.
    Tag engaged_target_tag;
    //! Buffs on this unit. Only valid for this player's units.
    std::vector<BuffID> buffs;

    Unit();

    operator Tag() const { return tag; }
};

typedef std::vector<Unit> Units;
typedef std::unordered_map<Tag, size_t> UnitIdxMap;

//! Determines if the unit matches the unit type.
struct IsUnit {
    IsUnit(UNIT_TYPEID type) : type_(type) {};
    UNIT_TYPEID type_;
    bool operator()(const Unit& unit) { return unit.unit_type == type_; };
};

//! Determines if units matches the unit type.
struct IsUnits {
    IsUnits(std::vector<UNIT_TYPEID> types) : types_(types) {};
    std::vector<UNIT_TYPEID> types_;
    bool operator()(const Unit& unit) {
        bool included = false;
        for (const auto& type : types_) {
            included = included || (unit.unit_type == type);
        }
        return included;
    };
};

}
