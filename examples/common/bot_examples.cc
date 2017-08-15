
#include "bot_examples.h"

#include <iostream>
#include <string>
#include <algorithm>
#include <random>
#include <iterator>

#include "sc2api/sc2_api.h"
#include "sc2lib/sc2_lib.h"

namespace sc2 {
    
static int TargetSCVCount = 15;

struct IsVisible {
    bool operator()(const Unit& unit) { return unit.display_type == Unit::Visible; };
};
struct IsAttackable {
    bool operator()(const Unit& unit) {
        switch (unit.unit_type.ToType()) {
            case UNIT_TYPEID::ZERG_OVERLORD: return false;
            case UNIT_TYPEID::ZERG_OVERSEER: return false;
            case UNIT_TYPEID::PROTOSS_OBSERVER: return false;
            default: return true;
        }
    }
};

struct IsFlying {
    bool operator()(const Unit& unit) {
        return unit.is_flying;
    }
};

//Ignores Overlords, workers, and structures
struct IsArmy {
    IsArmy(const ObservationInterface* obs) : observation_(obs) {}

    bool operator()(const Unit& unit) {
        auto attributes = observation_->GetUnitTypeData().at(unit.unit_type).attributes;
        for (const auto& attribute : attributes) {
            if (attribute == Attribute::Structure) {
                return false;
            }
        }
        switch (unit.unit_type.ToType()) {
            case UNIT_TYPEID::ZERG_OVERLORD: return false;
            case UNIT_TYPEID::PROTOSS_PROBE: return false;
            case UNIT_TYPEID::ZERG_DRONE: return false;
            case UNIT_TYPEID::TERRAN_SCV: return false;
            case UNIT_TYPEID::ZERG_QUEEN: return false;
            case UNIT_TYPEID::ZERG_LARVA: return false;
            case UNIT_TYPEID::ZERG_EGG: return false;
            case UNIT_TYPEID::TERRAN_MULE: return false;
            case UNIT_TYPEID::TERRAN_NUKE: return false;
            default: return true;
        }
    }

    const ObservationInterface* observation_;
};

struct IsTownHall {
    bool operator()(const Unit& unit) {
        switch (unit.unit_type.ToType()) {
            case UNIT_TYPEID::ZERG_HATCHERY: return true;
            case UNIT_TYPEID::ZERG_LAIR: return true;
            case UNIT_TYPEID::ZERG_HIVE : return true;
            case UNIT_TYPEID::TERRAN_COMMANDCENTER: return true;
            case UNIT_TYPEID::TERRAN_ORBITALCOMMAND: return true;
            case UNIT_TYPEID::TERRAN_ORBITALCOMMANDFLYING: return true;
            case UNIT_TYPEID::TERRAN_PLANETARYFORTRESS: return true;
            case UNIT_TYPEID::PROTOSS_NEXUS: return true;
            default: return false;
        }
    }
};

struct IsVespeneGeyser {
    bool operator()(const Unit& unit) {
        switch (unit.unit_type.ToType()) {
            case UNIT_TYPEID::NEUTRAL_VESPENEGEYSER: return true;
            case UNIT_TYPEID::NEUTRAL_SPACEPLATFORMGEYSER: return true;
            case UNIT_TYPEID::NEUTRAL_PROTOSSVESPENEGEYSER: return true;
            default: return false;
        }
    }
};

struct IsStructure {
    IsStructure(const ObservationInterface* obs) : observation_(obs) {};

    bool operator()(const Unit& unit) {
        auto& attributes = observation_->GetUnitTypeData().at(unit.unit_type).attributes;
        bool is_structure = false;
        for (const auto& attribute : attributes) {
            if (attribute == Attribute::Structure) {
                is_structure = true;
            }
        }
        return is_structure;
    }

    const ObservationInterface* observation_;
};

int CountUnitType(const ObservationInterface* observation, UnitTypeID unit_type) {
    int count = 0;
    Units my_units = observation->GetUnits(Unit::Alliance::Self);
    for (const Unit& unit : my_units) {
        if (unit.unit_type == unit_type)
            ++count;
    }

    return count;
}

bool FindEnemyStructure(const ObservationInterface* observation, Unit& enemy_unit) {
    Units my_units = observation->GetUnits(Unit::Alliance::Enemy);
    for (const Unit& unit : my_units) {
        if (unit.unit_type == UNIT_TYPEID::TERRAN_COMMANDCENTER ||
            unit.unit_type == UNIT_TYPEID::TERRAN_SUPPLYDEPOT ||
            unit.unit_type == UNIT_TYPEID::TERRAN_BARRACKS) {
            enemy_unit = unit;
            return true;
        }
    }

    return false;
}

bool GetRandomUnit(Unit& unit_out, const ObservationInterface* observation, UnitTypeID unit_type) {
    Units my_units = observation->GetUnits(Unit::Alliance::Self);
    std::random_shuffle(my_units.begin(), my_units.end()); // Doesn't work, or doesn't work well.
    for (const Unit& unit : my_units) {
        if (unit.unit_type == unit_type) {
            unit_out = unit;
            return true;
        }
    }
    return false;
}

void MultiplayerBot::PrintStatus(std::string msg) {
    int64_t bot_identifier = int64_t(this) & 0xFFFLL;
    std::cout << std::to_string(bot_identifier) << ": " << msg << std::endl;
}

void MultiplayerBot::OnGameStart() {
    game_info_ = Observation()->GetGameInfo();
    PrintStatus("game started.");
    expansions_ = search::CalculateExpansionLocations(Observation(), Query());

    //Temporary, we can replace this with observation->GetStartLocation() once implemented
    startLocation_ = Observation()->GetStartLocation();
    staging_location_ = startLocation_;
};

size_t MultiplayerBot::CountUnitType(const ObservationInterface* observation, UnitTypeID unit_type) {
    return observation->GetUnits(Unit::Alliance::Self, IsUnit(unit_type)).size();
}

size_t MultiplayerBot::CountUnitTypeBuilding(const ObservationInterface* observation, UNIT_TYPEID production_building, ABILITY_ID ability) {
    int building_count = 0;
    Units buildings = observation->GetUnits(Unit::Self, IsUnit(production_building));

    for (const auto& building : buildings) {
        if (building.orders.empty()) {
            continue;
        }

        for (const auto order : building.orders) {
            if (order.ability_id == ability) {
                building_count++;
            }
        }
    }

    return building_count;
}

size_t MultiplayerBot::CountUnitTypeTotal(const ObservationInterface* observation, UNIT_TYPEID unit_type, UNIT_TYPEID production, ABILITY_ID ability) {
    return CountUnitType(observation, unit_type) + CountUnitTypeBuilding(observation, production, ability);
}

size_t MultiplayerBot::CountUnitTypeTotal(const ObservationInterface* observation, std::vector<UNIT_TYPEID> unit_type, UNIT_TYPEID production, ABILITY_ID ability) {
    size_t count = 0;
    for (const auto& type : unit_type) {
        count += CountUnitType(observation, type);
    }
    return count + CountUnitTypeBuilding(observation, production, ability);
}

bool MultiplayerBot::GetRandomUnit(Unit& unit_out, const ObservationInterface* observation, UnitTypeID unit_type) {
    Units my_units = observation->GetUnits(Unit::Alliance::Self, IsUnit(unit_type));
    if (!my_units.empty()) {
        unit_out = GetRandomEntry(my_units);
        return true;
    }
    return false;
}

bool MultiplayerBot::FindNearestMineralPatch(const Point2D& start, Tag& target) {
    Units units = Observation()->GetUnits(Unit::Alliance::Neutral);
    float distance = std::numeric_limits<float>::max();
    for (const auto& u : units) {
        if (u.unit_type == UNIT_TYPEID::NEUTRAL_MINERALFIELD) {
            float d = DistanceSquared2D(u.pos, start);
            if (d < distance) {
                distance = d;
                target = u.tag;
            }
        }
    }
    //If we never found one return false;
    if (distance == std::numeric_limits<float>::max()) {
        return false;
    }
    return true;
}

// Tries to find a random location that can be pathed to on the map.
// Returns 'true' if a new, random location has been found that is pathable by the unit.
bool MultiplayerBot::FindEnemyPosition(Tag, Point2D& target_pos) {
    if (game_info_.enemy_start_locations.empty()) {
        return false;
    }
    target_pos = game_info_.enemy_start_locations.front();
    return true;
}

bool MultiplayerBot::TryFindRandomPathableLocation(Tag tag, Point2D& target_pos) {
    // First, find a random point inside the playable area of the map.
    float playable_w = game_info_.playable_max.x - game_info_.playable_min.x;
    float playable_h = game_info_.playable_max.y - game_info_.playable_min.y;

    // The case where game_info_ does not provide a valid answer
    if (playable_w == 0 || playable_h == 0) {
        playable_w = 236;
        playable_h = 228;
    }

    target_pos.x = playable_w * GetRandomFraction() + game_info_.playable_min.x;
    target_pos.y = playable_h * GetRandomFraction() + game_info_.playable_min.y;

    // Now send a pathing query from the unit to that point. Can also query from point to point,
    // but using a unit tag wherever possible will be more accurate.
    // Note: This query must communicate with the game to get a result which affects performance.
    // Ideally batch up the queries (using PathingDistanceBatched) and do many at once.
    float distance = Query()->PathingDistance(tag, target_pos);

    return distance > 0.1f;
}

void MultiplayerBot::AttackWithUnitType(UnitTypeID unit_type, const ObservationInterface* observation) {
    Units units = observation->GetUnits(Unit::Alliance::Self, IsUnit(unit_type));
    for (const auto& unit : units) {
        AttackWithUnit(unit, observation);
    }
}

void MultiplayerBot::AttackWithUnit(Unit unit, const ObservationInterface* observation) {
    //If unit isn't doing anything make it attack.
    Units enemy_units = observation->GetUnits(Unit::Alliance::Enemy);
    if (enemy_units.empty()) {
        return;
    }

    if (unit.orders.empty()) {
        Actions()->UnitCommand(unit, ABILITY_ID::ATTACK, enemy_units.front().pos);
        return;
    }

    //If the unit is doing something besides attacking, make it attack.
    if (unit.orders.front().ability_id != ABILITY_ID::ATTACK) {
        Actions()->UnitCommand(unit, ABILITY_ID::ATTACK, enemy_units.front().pos);
    }
}

void MultiplayerBot::ScoutWithUnits(UnitTypeID unit_type, const ObservationInterface* observation) {
    Units units = observation->GetUnits(Unit::Alliance::Self, IsUnit(unit_type));
    for (const auto& unit : units) {
        ScoutWithUnit(unit, observation);
    }
}

void MultiplayerBot::ScoutWithUnit(Unit unit, const ObservationInterface* observation) {
    Units enemy_units = observation->GetUnits(Unit::Alliance::Enemy, IsAttackable());
    if (!unit.orders.empty()) {
        return;
    }
    Point2D target_pos;

    if (FindEnemyPosition(unit.tag, target_pos)) {
        if (Distance2D(unit.pos, target_pos) < 20 && enemy_units.empty()) {
            if (TryFindRandomPathableLocation(unit.tag, target_pos)) {
                Actions()->UnitCommand(unit, ABILITY_ID::SMART, target_pos);
                return;
            }
        }
        else if (!enemy_units.empty())
        {
            Actions()->UnitCommand(unit, ABILITY_ID::ATTACK, enemy_units.front());
            return;
        }
        Actions()->UnitCommand(unit, ABILITY_ID::SMART, target_pos);
    }
    else {
        if (TryFindRandomPathableLocation(unit.tag, target_pos)) {
            Actions()->UnitCommand(unit, ABILITY_ID::SMART, target_pos);
        }
    }
}

//Try build structure given a location. This is used most of the time
bool MultiplayerBot::TryBuildStructure(AbilityID ability_type_for_structure, UnitTypeID unit_type, Point2D location, bool isExpansion = false) {

    const ObservationInterface* observation = Observation();
    Units workers = observation->GetUnits(Unit::Alliance::Self, IsUnit(unit_type));

    //if we have no workers Don't build
    if (workers.empty()) {
        return false;
    }

    // Check to see if there is already a worker heading out to build it
    for (const auto& worker : workers) {
        for (const auto& order : worker.orders) {
            if (order.ability_id == ability_type_for_structure) {
                return false;
            }
        }
    }

    // If no worker is already building one, get a random worker to build one
    const Unit& unit = GetRandomEntry(workers);

    // Check to see if unit can make it there
    if (Query()->PathingDistance(unit.tag, location) < 0.1f) {
        return false;
    }
    if (!isExpansion) {
        for (const auto& expansion : expansions_) {
            if (Distance2D(location, Point2D(expansion.x, expansion.y)) < 7) {
                return false;
            }
        }
    }
    // Check to see if unit can build there
    if (Query()->Placement(ability_type_for_structure, location)) {
        Actions()->UnitCommand(unit.tag, ability_type_for_structure, location);
        return true;
    }
    return false;

}

//Try to build a structure based on tag, Used mostly for Vespene, since the pathing check will fail even though the geyser is "Pathable"
bool MultiplayerBot::TryBuildStructure(AbilityID ability_type_for_structure, UnitTypeID unit_type, Tag location_tag) {
    const ObservationInterface* observation = Observation();
    Units workers = observation->GetUnits(Unit::Alliance::Self, IsUnit(unit_type));
    const Unit* target = observation->GetUnit(location_tag);

    if (workers.empty()) {
        return false;
    }

    // Check to see if there is already a worker heading out to build it
    for (const auto& worker : workers) {
        for (const auto& order : worker.orders) {
            if (order.ability_id == ability_type_for_structure) {
                return false;
            }
        }
    }

    // If no worker is already building one, get a random worker to build one
    const Unit& unit = GetRandomEntry(workers);

    // Check to see if unit can build there
    if (Query()->Placement(ability_type_for_structure, target->pos)) {
        Actions()->UnitCommand(unit.tag, ability_type_for_structure, location_tag);
        return true;
    }
    return false;

}

//Expands to nearest location and updates the start location to be between the new location and old bases.
bool MultiplayerBot::TryExpand(AbilityID build_ability, UnitTypeID worker_type) {
    const ObservationInterface* observation = Observation();
    float minimum_distance = std::numeric_limits<float>::max();
    Point3D closest_expansion;
    for (const auto& expansion : expansions_) {
        float current_distance = Distance2D(startLocation_, expansion);
        if (current_distance < .01f) {
            continue;
        }

        if (current_distance < minimum_distance) {
            if (Query()->Placement(build_ability, expansion)) {
                closest_expansion = expansion;
                minimum_distance = current_distance;
            }
        }
    }
    //only update staging location up till 3 bases.
    if (TryBuildStructure(build_ability, worker_type, closest_expansion, true) && observation->GetUnits(Unit::Self, IsTownHall()).size() < 4) {
        staging_location_ = Point3D(((staging_location_.x + closest_expansion.x) / 2), ((staging_location_.y + closest_expansion.y) / 2),
            ((staging_location_.z + closest_expansion.z) / 2));
        return true;
    }
    return false;

}

//Tries to build a geyser for a base
bool MultiplayerBot::TryBuildGas(AbilityID build_ability, UnitTypeID worker_type, Point2D base_location) {
    const ObservationInterface* observation = Observation();
    Units geysers = observation->GetUnits(Unit::Alliance::Neutral, IsVespeneGeyser());

    //only search within this radius
    float minimum_distance = 15.0f;
    Tag closestGeyser = 0;
    for (const auto& geyser : geysers) {
        float current_distance = Distance2D(base_location, geyser.pos);
        if (current_distance < minimum_distance) {
            if (Query()->Placement(build_ability, geyser.pos)) {
                minimum_distance = current_distance;
                closestGeyser = geyser.tag;
            }
        }
    }

    // In the case where there are no more available geysers nearby
    if (closestGeyser == 0) {
        return false;
    }
    return TryBuildStructure(build_ability, worker_type, closestGeyser);

}

bool MultiplayerBot::TryBuildUnit(AbilityID ability_type_for_unit, UnitTypeID unit_type) {
    const ObservationInterface* observation = Observation();

    //If we are at supply cap, don't build anymore units, unless its an overlord.
    if (observation->GetFoodUsed() >= observation->GetFoodCap() && ability_type_for_unit != ABILITY_ID::TRAIN_OVERLORD) {
        return false;
    }
    Unit unit;
    if (!GetRandomUnit(unit, observation, unit_type)) {
        return false;
    }
    if (!unit.orders.empty()) {
        return false;
    }

    if (unit.build_progress != 1) {
        return false;
    }

    Actions()->UnitCommand(unit.tag, ability_type_for_unit);
    return true;
}

// Mine the nearest mineral to Town hall.
// If we don't do this, probes may mine from other patches if they stray too far from the base after building.
void MultiplayerBot::MineIdleWorkers(Tag worker_tag, AbilityID worker_gather_command, UnitTypeID vespene_building_type) {
    const ObservationInterface* observation = Observation();
    Units bases = observation->GetUnits(Unit::Alliance::Self, IsTownHall());
    Units geysers = observation->GetUnits(Unit::Alliance::Self, IsUnit(vespene_building_type));

    Tag valid_mineral_patch;

    if (bases.empty()) {
        return;
    }

    for (const auto& geyser : geysers) {
        if (geyser.assigned_harvesters < geyser.ideal_harvesters) {
            Actions()->UnitCommand(worker_tag, worker_gather_command, geyser.tag);
            return;
        }
    }
    //Search for a base that is missing workers.
    for (const auto& base : bases) {
        //If we have already mined out here skip the base.
        if (base.ideal_harvesters == 0 || base.build_progress != 1) {
            continue;
        }
        if (base.assigned_harvesters < base.ideal_harvesters) {
            FindNearestMineralPatch(base.pos, valid_mineral_patch);
            Actions()->UnitCommand(worker_tag, worker_gather_command, valid_mineral_patch);
            return;
        }
    }

    if (!observation->GetUnit(worker_tag)->orders.empty()) {
        return;
    }

    //If all workers are spots are filled just go to any base.
    const Unit& random_base = GetRandomEntry(bases);
    FindNearestMineralPatch(random_base.pos, valid_mineral_patch);
    Actions()->UnitCommand(worker_tag, worker_gather_command, valid_mineral_patch);
}

//An estimate of how many workers we should have based on what buildings we have
int MultiplayerBot::GetExpectedWorkers(UNIT_TYPEID vespene_building_type) {
    const ObservationInterface* observation = Observation();
    Units bases = observation->GetUnits(Unit::Alliance::Self, IsTownHall());
    Units geysers = observation->GetUnits(Unit::Alliance::Self, IsUnit(vespene_building_type));
    int expected_workers = 0;
    for (const auto& base : bases) {
        if (base.build_progress != 1) {
            continue;
        }
        expected_workers += base.ideal_harvesters;
    }

    for (const auto& geyser : geysers) {
        if (geyser.vespene_contents > 0) {
            if (geyser.build_progress != 1) {
                continue;
            }
            expected_workers += geyser.ideal_harvesters;
        }
    }

    return expected_workers;
}

// To ensure that we do not over or under saturate any base.
void MultiplayerBot::ManageWorkers(UNIT_TYPEID worker_type, AbilityID worker_gather_command, UNIT_TYPEID vespene_building_type) {
    const ObservationInterface* observation = Observation();
    Units bases = observation->GetUnits(Unit::Alliance::Self, IsTownHall());
    Units geysers = observation->GetUnits(Unit::Alliance::Self, IsUnit(vespene_building_type));

    if (bases.empty()) {
        return;
    }

    for (const auto& base : bases) {
        //If we have already mined out or still building here skip the base.
        if (base.ideal_harvesters == 0 || base.build_progress != 1) {
            continue;
        }
        //if base is
        if (base.assigned_harvesters > base.ideal_harvesters) {
            Units workers = observation->GetUnits(Unit::Alliance::Self, IsUnit(worker_type));

            for (const auto& worker : workers) {
                if (!worker.orders.empty()) {
                    if (worker.orders.front().target_unit_tag == base.tag) {
                        //This should allow them to be picked up by mineidleworkers()
                        MineIdleWorkers(worker.tag, worker_gather_command,vespene_building_type);
                        return;
                    }
                }
            }
        }
    }
    Units workers = observation->GetUnits(Unit::Alliance::Self, IsUnit(worker_type));
    for (const auto& geyser : geysers) {
        if (geyser.ideal_harvesters == 0 || geyser.build_progress != 1) {
            continue;
        }
        if (geyser.assigned_harvesters > geyser.ideal_harvesters) {
            for (const auto& worker : workers) {
                if (!worker.orders.empty()) {
                    if (worker.orders.front().target_unit_tag == geyser.tag) {
                        //This should allow them to be picked up by mineidleworkers()
                        MineIdleWorkers(worker.tag, worker_gather_command, vespene_building_type);
                        return;
                    }
                }
            }
        }
        else if (geyser.assigned_harvesters < geyser.ideal_harvesters) {
            for (const auto& worker : workers) {
                if (!worker.orders.empty()) {
                    //This should move a worker that isn't mining gas to gas
                    const Unit* target = observation->GetUnit(worker.orders.front().target_unit_tag);
                    if (target == nullptr) {
                        continue;
                    }
                    if (target->unit_type != vespene_building_type) {
                        //This should allow them to be picked up by mineidleworkers()
                        MineIdleWorkers(worker.tag, worker_gather_command, vespene_building_type);
                        return;
                    }
                }
            }
        }
    }
}

void MultiplayerBot::RetreatWithUnits(UnitTypeID unit_type, Point2D retreat_position) {
    const ObservationInterface* observation = Observation();
    Units units = observation->GetUnits(Unit::Alliance::Self, IsUnit(unit_type));
    for (const auto& unit : units) {
        RetreatWithUnit(unit, retreat_position);
    }
}

void MultiplayerBot::RetreatWithUnit(Unit unit, Point2D retreat_position) {
    float dist = Distance2D(unit.pos, retreat_position);

    if (dist < 10) {
        if (unit.orders.empty()) {
            return;
        }
        Actions()->UnitCommand(unit, ABILITY_ID::STOP);
        return;
    }

    if (unit.orders.empty() && dist > 14) {
        Actions()->UnitCommand(unit, ABILITY_ID::MOVE, retreat_position);
    }
    else if (!unit.orders.empty() && dist > 14) {
        if (unit.orders.front().ability_id != ABILITY_ID::MOVE) {
            Actions()->UnitCommand(unit, ABILITY_ID::MOVE, retreat_position);
        }
    }
}

void MultiplayerBot::OnNuclearLaunchDetected() {
    const ObservationInterface* observation = Observation();
    nuke_detected = true;
    nuke_detected_frame = observation->GetGameLoop();
}
//Manages attack and retreat patterns, as well as unit micro
void ProtossMultiplayerBot::ManageArmy() {
    const ObservationInterface* observation = Observation();
    Units enemy_units = observation->GetUnits(Unit::Alliance::Enemy);
    Units army = observation->GetUnits(Unit::Alliance::Self, IsArmy(observation));
    int wait_til_supply = 100;

    //There are no enemies yet, and we don't have a big army
    if (enemy_units.empty() && observation->GetFoodArmy() < wait_til_supply) {
        for (const auto& unit : army) {
            RetreatWithUnit(unit, staging_location_);
        }
    }
    else if(!enemy_units.empty()){
        for (const auto& unit : army) {
            AttackWithUnit(unit, observation);
            switch (unit.unit_type.ToType()) {
                //Stalker blink micro, blinks back towards your base
                case(UNIT_TYPEID::PROTOSS_OBSERVER): {
                    Actions()->UnitCommand(unit.tag, ABILITY_ID::ATTACK, enemy_units.front().pos);
                    break;
                }
                case(UNIT_TYPEID::PROTOSS_STALKER): {
                    if (blink_reasearched_) {
                        const Unit* old_unit = observation->GetPreviousUnit(unit.tag);
                        const Unit* target_unit = observation->GetUnit(unit.engaged_target_tag);
                        if (old_unit == nullptr) {
                            break;
                        }
                        Point2D blink_location = startLocation_;
                        if (old_unit->shield > 0 && unit.shield < 1) {
                            if (!unit.orders.empty()) {
                                if (target_unit != nullptr) {
                                    Vector2D diff = unit.pos - target_unit->pos;
                                    Normalize2D(diff);
                                    blink_location = unit.pos + diff * 7.0f;
                                }
                                else {
                                    Vector2D diff = unit.pos - startLocation_;
                                    Normalize2D(diff);
                                    blink_location = unit.pos - diff * 7.0f;
                                }
                                Actions()->UnitCommand(unit.tag, ABILITY_ID::EFFECT_BLINK, blink_location);
                            }
                        }
                    }
                    break;
                }
                //Turns on guardian shell when close to an enemy
                case (UNIT_TYPEID::PROTOSS_SENTRY): {
                    if (!unit.orders.empty()) {
                        if (unit.orders.front().ability_id == ABILITY_ID::ATTACK) {
                            float distance = std::numeric_limits<float>::max();
                            for (const auto& u : enemy_units) {
                                 float d = Distance2D(u.pos, unit.pos);
                                 if (d < distance) {
                                     distance = d;
                                 }
                            }
                            if (distance < 6 && unit.energy >= 75) {
                                Actions()->UnitCommand(unit.tag, ABILITY_ID::EFFECT_GUARDIANSHIELD);
                            }
                        }
                    }
                    break;
                }
                //Turns on Damage boost when close to an enemy
                case (UNIT_TYPEID::PROTOSS_VOIDRAY): {
                    if (!unit.orders.empty()) {
                        if (unit.orders.front().ability_id == ABILITY_ID::ATTACK) {
                            float distance = std::numeric_limits<float>::max();
                            for (const auto& u : enemy_units) {
                                float d = Distance2D(u.pos, unit.pos);
                                if (d < distance) {
                                    distance = d;
                                }
                            }
                            if (distance < 8) {
                                Actions()->UnitCommand(unit.tag, ABILITY_ID::EFFECT_VOIDRAYPRISMATICALIGNMENT);
                            }
                        }
                    }
                    break;
                }
                //Turns on oracle weapon when close to an enemy
                case (UNIT_TYPEID::PROTOSS_ORACLE): {
                    if (!unit.orders.empty()) {
                         float distance = std::numeric_limits<float>::max();
                         for (const auto& u : enemy_units) {
                             float d = Distance2D(u.pos, unit.pos);
                             if (d < distance) {
                                 distance = d;
                             }
                         }
                         if (distance < 6 && unit.energy >= 25) {
                             Actions()->UnitCommand(unit.tag, ABILITY_ID::BEHAVIOR_PULSARBEAMON);
                         }
                    }
                    break;
                }
                //fires a disruptor nova when in range
                case (UNIT_TYPEID::PROTOSS_DISRUPTOR): {
                    float distance = std::numeric_limits<float>::max();
                    Point2D closest_unit;
                    for (const auto& u : enemy_units) {
                        float d = Distance2D(u.pos, unit.pos);
                        if (d < distance) {
                            distance = d;
                            closest_unit = u.pos;
                        }
                    }
                    if (distance < 7) {
                        Actions()->UnitCommand(unit.tag, ABILITY_ID::EFFECT_PURIFICATIONNOVA, closest_unit);
                    }
                    else {
                        Actions()->UnitCommand(unit.tag, ABILITY_ID::ATTACK, closest_unit);
                    }
                    break;
                }
                //controls disruptor novas.
                case (UNIT_TYPEID::PROTOSS_DISRUPTORPHASED): {
                            float distance = std::numeric_limits<float>::max();
                            Point2D closest_unit;
                            for (const auto& u : enemy_units) {
                                if (u.is_flying) {
                                    continue;
                                }
                                float d = DistanceSquared2D(u.pos, unit.pos);
                                if (d < distance) {
                                    distance = d;
                                    closest_unit = u.pos;
                                }
                            }
                            Actions()->UnitCommand(unit.tag, ABILITY_ID::MOVE, closest_unit);
                    break;
                }
                default:
                    break;
            }
        }
    }
    else {
        for (const auto& unit : army) {
            ScoutWithUnit(unit, observation);
        }
    }
}

//Manages when to build and how many to build of units
bool ProtossMultiplayerBot::TryBuildArmy() {
    const ObservationInterface* observation = Observation();
    if (observation->GetFoodWorkers() <= target_worker_count_) {
        return false;
    }
    //Until we have 2 bases, hold off on building too many units
    if (CountUnitType(observation, UNIT_TYPEID::PROTOSS_NEXUS) < 2 && observation->GetFoodArmy() > 10) {
        return false;
    }

    //If we have a decent army already, try hold until we expand again
    if (CountUnitType(observation, UNIT_TYPEID::PROTOSS_NEXUS) < 3 && observation->GetFoodArmy() > 40) {
        return false;
    }
    size_t mothership_count = CountUnitType(observation, UNIT_TYPEID::PROTOSS_MOTHERSHIPCORE);
    mothership_count += CountUnitType(observation, UNIT_TYPEID::PROTOSS_MOTHERSHIP);

    if (observation->GetFoodWorkers() > target_worker_count_ && mothership_count < 1) {
        if (CountUnitType(observation, UNIT_TYPEID::PROTOSS_CYBERNETICSCORE) > 0) {
            TryBuildUnit(ABILITY_ID::TRAIN_MOTHERSHIPCORE, UNIT_TYPEID::PROTOSS_NEXUS);
        }
    }
    size_t colossus_count = CountUnitType(observation, UNIT_TYPEID::PROTOSS_COLOSSUS);
    size_t carrier_count = CountUnitType(observation, UNIT_TYPEID::PROTOSS_CARRIER);
    Units templar = observation->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::PROTOSS_HIGHTEMPLAR));
    if (templar.size() > 1) {
        std::vector<Tag> templar_merge;
        for (int i = 0; i < 2; ++i) {
            templar_merge.push_back(templar.at(i).tag);
        }
        Actions()->UnitCommand(templar_merge, ABILITY_ID::MORPH_ARCHON);
    }

    if (air_build_) {
        //If we have a fleet beacon, and haven't hit our carrier count, build more carriers
        if (CountUnitType(observation, UNIT_TYPEID::PROTOSS_FLEETBEACON) > 0 && carrier_count < max_colossus_count_) {
            //After the first carrier try and make a Mothership
            if (CountUnitType(observation, UNIT_TYPEID::PROTOSS_MOTHERSHIP) < 1 && mothership_count > 0) {
                if (TryBuildUnit(ABILITY_ID::MORPH_MOTHERSHIP, UNIT_TYPEID::PROTOSS_MOTHERSHIPCORE)) {
                    return true;
                }
                return false;
            }

            if (observation->GetMinerals() > 350 && observation->GetVespene() > 250) {
                if (TryBuildUnit(ABILITY_ID::TRAIN_CARRIER, UNIT_TYPEID::PROTOSS_STARGATE)) {
                    return true;
                }
            }
            else if (CountUnitType(observation, UNIT_TYPEID::PROTOSS_TEMPEST) < 1) {
                TryBuildUnit(ABILITY_ID::TRAIN_TEMPEST, UNIT_TYPEID::PROTOSS_STARGATE);
            }
            else if (carrier_count < 1){ //Try to build at least 1
                return false;
            }
        }
        else {
            // If we can't build Carrier, try to build voidrays
            if (observation->GetMinerals() > 250 && observation->GetVespene() > 150) {
                TryBuildUnit(ABILITY_ID::TRAIN_VOIDRAY, UNIT_TYPEID::PROTOSS_STARGATE);

            }
            //Have at least 1 void ray before we build the other units
            if (CountUnitType(observation, UNIT_TYPEID::PROTOSS_VOIDRAY) > 0) {
                if (CountUnitType(observation, UNIT_TYPEID::PROTOSS_ORACLE) < 1) {
                    TryBuildUnit(ABILITY_ID::TRAIN_ORACLE, UNIT_TYPEID::PROTOSS_STARGATE);
                }
                else if (CountUnitType(observation, UNIT_TYPEID::PROTOSS_PHOENIX) < 2) {
                    TryBuildUnit(ABILITY_ID::TRAIN_PHOENIX, UNIT_TYPEID::PROTOSS_STARGATE);
                }
            }
        }
    }
    else {
        //If we have a robotics bay, and haven't hit our colossus count, build more colossus
        if (CountUnitType(observation, UNIT_TYPEID::PROTOSS_OBSERVER) < 1) {
            TryBuildUnit(ABILITY_ID::TRAIN_OBSERVER, UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY);
        }
        if (CountUnitType(observation, UNIT_TYPEID::PROTOSS_ROBOTICSBAY) > 0 && colossus_count < max_colossus_count_) {
            if (observation->GetMinerals() > 300 && observation->GetVespene() > 200) {
                if (TryBuildUnit(ABILITY_ID::TRAIN_COLOSSUS, UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY)) {
                    return true;
                }
            }
            else if (CountUnitTypeTotal(observation, UNIT_TYPEID::PROTOSS_DISRUPTOR, UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY, ABILITY_ID::TRAIN_DISRUPTOR) < 2) {
                TryBuildUnit(ABILITY_ID::TRAIN_DISRUPTOR, UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY);
            }
        }
        else {
            // If we can't build Colossus, try to build immortals
            if (observation->GetMinerals() > 250 && observation->GetVespene() > 100) {
                if (TryBuildUnit(ABILITY_ID::TRAIN_IMMORTAL, UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY)) {
                    return true;
                }
            }
        }
    }

    if (warpgate_reasearched_ &&  CountUnitType(observation, UNIT_TYPEID::PROTOSS_WARPGATE) > 0) {
        if (observation->GetMinerals() > 1000 && observation->GetVespene() < 200) {
            return TryWarpInUnit(ABILITY_ID::TRAINWARP_ZEALOT);
        }
        if (CountUnitType(observation, UNIT_TYPEID::PROTOSS_STALKER) > max_stalker_count_){
            return false;
        }

        if (CountUnitType(observation, UNIT_TYPEID::PROTOSS_ADEPT) > max_stalker_count_) {
            return false;
        }

        if (!air_build_) {
            if (CountUnitType(observation, UNIT_TYPEID::PROTOSS_SENTRY) < max_sentry_count_) {
                return TryWarpInUnit(ABILITY_ID::TRAINWARP_SENTRY);
            }

            if (CountUnitType(observation, UNIT_TYPEID::PROTOSS_HIGHTEMPLAR) < 2 && CountUnitType(observation, UNIT_TYPEID::PROTOSS_ARCHON) < 2) {
                return TryWarpInUnit(ABILITY_ID::TRAINWARP_HIGHTEMPLAR);
            }
        }
        //build adepts until we have robotics facility, then switch to stalkers.
        if (CountUnitType(observation, UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY) > 0) {
            return TryWarpInUnit(ABILITY_ID::TRAINWARP_STALKER);
        }
        else {
            return TryWarpInUnit(ABILITY_ID::TRAINWARP_ADEPT);
        }
    }
    else {
        //Train Adepts if we have a core otherwise build Zealots
        if (observation->GetMinerals() > 120 && observation->GetVespene() > 25) {
            if (CountUnitType(observation, UNIT_TYPEID::PROTOSS_CYBERNETICSCORE) > 0) {
                return TryBuildUnit(ABILITY_ID::TRAIN_ADEPT, UNIT_TYPEID::PROTOSS_GATEWAY);
            }
        }
        else if (observation->GetMinerals() > 100) {
            return TryBuildUnit(ABILITY_ID::TRAIN_ZEALOT, UNIT_TYPEID::PROTOSS_GATEWAY);
        }
        return false;
    }
}

//Manages when to build your buildings
void ProtossMultiplayerBot::BuildOrder() {
    const ObservationInterface* observation = Observation();
    size_t gateway_count = CountUnitType(observation, UNIT_TYPEID::PROTOSS_GATEWAY) + CountUnitType(observation,UNIT_TYPEID::PROTOSS_WARPGATE);
    size_t cybernetics_count = CountUnitType(observation, UNIT_TYPEID::PROTOSS_CYBERNETICSCORE);
    size_t forge_count = CountUnitType(observation, UNIT_TYPEID::PROTOSS_FORGE);
    size_t twilight_council_count = CountUnitType(observation, UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL);
    size_t templar_archive_count = CountUnitType(observation, UNIT_TYPEID::PROTOSS_TEMPLARARCHIVE);
    size_t base_count = CountUnitType(observation, UNIT_TYPEID::PROTOSS_NEXUS);
    size_t robotics_facility_count = CountUnitType(observation, UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY);
    size_t robotics_bay_count = CountUnitType(observation, UNIT_TYPEID::PROTOSS_ROBOTICSBAY);
    size_t stargate_count = CountUnitType(observation, UNIT_TYPEID::PROTOSS_STARGATE);
    size_t fleet_beacon_count = CountUnitType(observation, UNIT_TYPEID::PROTOSS_FLEETBEACON);

    // 3 Gateway per expansion
    if (gateway_count < std::min<size_t>(2 * base_count, 7)) {

        //If we have 1 gateway, prioritize building CyberCore
        if (cybernetics_count < 1 && gateway_count > 0) {
            TryBuildStructureNearPylon(ABILITY_ID::BUILD_CYBERNETICSCORE, UNIT_TYPEID::PROTOSS_PROBE);
            return;
        }
        else {
            //If we have 1 gateway Prioritize getting another expansion out before building more gateways
            if (base_count < 2 && gateway_count > 0) {
                TryBuildExpansionNexus();
                return;
            }

            if (observation->GetFoodWorkers() >= target_worker_count_  && observation->GetMinerals() > 150 + (100 * gateway_count)) {
                TryBuildStructureNearPylon(ABILITY_ID::BUILD_GATEWAY, UNIT_TYPEID::PROTOSS_PROBE);
            }
        }
    }

    if (cybernetics_count > 0 && forge_count < 2) {
        TryBuildStructureNearPylon(ABILITY_ID::BUILD_FORGE, UNIT_TYPEID::PROTOSS_PROBE);
    }

    //go stargate or robo depending on build
    if (air_build_) {
        if (gateway_count > 1 && cybernetics_count > 0) {
            if (stargate_count < std::min<size_t>(base_count, 5)) {
                if (observation->GetMinerals() > 150 && observation->GetVespene() > 150) {
                    TryBuildStructureNearPylon(ABILITY_ID::BUILD_STARGATE, UNIT_TYPEID::PROTOSS_PROBE);
                }
            }
            else if (stargate_count > 0 && fleet_beacon_count < 1) {
                if (observation->GetMinerals() > 300 && observation->GetVespene() > 200) {
                    TryBuildStructureNearPylon(ABILITY_ID::BUILD_FLEETBEACON, UNIT_TYPEID::PROTOSS_PROBE);
                }
            }
        }
    }
    else {
        if (gateway_count > 2 && cybernetics_count > 0) {
            if (robotics_facility_count < std::min<size_t>(base_count, 4)) {
                if (observation->GetMinerals() > 200 && observation->GetVespene() > 100) {
                    TryBuildStructureNearPylon(ABILITY_ID::BUILD_ROBOTICSFACILITY, UNIT_TYPEID::PROTOSS_PROBE);
                }
            }
            else if (robotics_facility_count > 0 && robotics_bay_count < 1) {
                if (observation->GetMinerals() > 200 && observation->GetVespene() > 200) {
                    TryBuildStructureNearPylon(ABILITY_ID::BUILD_ROBOTICSBAY, UNIT_TYPEID::PROTOSS_PROBE);
                }
            }
        }
    }

    if (forge_count > 0 && twilight_council_count < 1 && base_count > 1) {
        TryBuildStructureNearPylon(ABILITY_ID::BUILD_TWILIGHTCOUNCIL, UNIT_TYPEID::PROTOSS_PROBE);
    }

    if (twilight_council_count > 0 && templar_archive_count < 1 && base_count > 2) {
        TryBuildStructureNearPylon(ABILITY_ID::BUILD_TEMPLARARCHIVE, UNIT_TYPEID::PROTOSS_PROBE);
    }

}

//Try to get upgrades depending on build
void ProtossMultiplayerBot::ManageUpgrades() {
    const ObservationInterface* observation = Observation();
    auto upgrades = observation->GetUpgrades();
    size_t base_count = observation->GetUnits(Unit::Alliance::Self, IsTownHall()).size();
    if (upgrades.empty()) {
        TryBuildUnit(ABILITY_ID::RESEARCH_WARPGATE, UNIT_TYPEID::PROTOSS_CYBERNETICSCORE);
    }
    else {
        for (const auto& upgrade : upgrades) {
            if (air_build_) {
                if (upgrade == UPGRADE_ID::PROTOSSAIRWEAPONSLEVEL1 && base_count > 2) {
                    TryBuildUnit(ABILITY_ID::RESEARCH_PROTOSSAIRWEAPONS, UNIT_TYPEID::PROTOSS_CYBERNETICSCORE);
                }
                else if (upgrade == UPGRADE_ID::PROTOSSAIRARMORSLEVEL1 && base_count > 2) {
                    TryBuildUnit(ABILITY_ID::RESEARCH_PROTOSSAIRARMOR, UNIT_TYPEID::PROTOSS_CYBERNETICSCORE);
                }
                else if (upgrade == UPGRADE_ID::PROTOSSSHIELDSLEVEL1 && base_count > 2) {
                    TryBuildUnit(ABILITY_ID::RESEARCH_PROTOSSSHIELDS, UNIT_TYPEID::PROTOSS_FORGE);
                }
                else if (upgrade == UPGRADE_ID::PROTOSSAIRARMORSLEVEL2 && base_count > 3) {
                    TryBuildUnit(ABILITY_ID::RESEARCH_PROTOSSAIRARMOR, UNIT_TYPEID::PROTOSS_CYBERNETICSCORE);
                }
                else if (upgrade == UPGRADE_ID::PROTOSSAIRWEAPONSLEVEL2 && base_count > 2) {
                    TryBuildUnit(ABILITY_ID::RESEARCH_PROTOSSAIRWEAPONS, UNIT_TYPEID::PROTOSS_CYBERNETICSCORE);
                }
                else if (upgrade == UPGRADE_ID::PROTOSSSHIELDSLEVEL2 && base_count > 2) {
                    TryBuildUnit(ABILITY_ID::RESEARCH_PROTOSSSHIELDS, UNIT_TYPEID::PROTOSS_FORGE);
                }
            }
            if (upgrade == UPGRADE_ID::PROTOSSGROUNDWEAPONSLEVEL1 && base_count > 2) {
                TryBuildUnit(ABILITY_ID::RESEARCH_PROTOSSGROUNDWEAPONS, UNIT_TYPEID::PROTOSS_FORGE);
            }
            else if (upgrade == UPGRADE_ID::PROTOSSGROUNDARMORSLEVEL1 && base_count > 2) {
                TryBuildUnit(ABILITY_ID::RESEARCH_PROTOSSGROUNDARMOR, UNIT_TYPEID::PROTOSS_FORGE);
            }
            else if (upgrade == UPGRADE_ID::PROTOSSGROUNDWEAPONSLEVEL2 && base_count > 3) {
                TryBuildUnit(ABILITY_ID::RESEARCH_PROTOSSGROUNDWEAPONS, UNIT_TYPEID::PROTOSS_FORGE);
            }
            else if (upgrade == UPGRADE_ID::PROTOSSGROUNDARMORSLEVEL2 && base_count > 3) {
                TryBuildUnit(ABILITY_ID::RESEARCH_PROTOSSGROUNDARMOR, UNIT_TYPEID::PROTOSS_FORGE);
            }
            else {
                if (air_build_) {
                    TryBuildUnit(ABILITY_ID::RESEARCH_PROTOSSAIRARMOR, UNIT_TYPEID::PROTOSS_CYBERNETICSCORE);
                    TryBuildUnit(ABILITY_ID::RESEARCH_PROTOSSAIRWEAPONS, UNIT_TYPEID::PROTOSS_CYBERNETICSCORE);
                    TryBuildUnit(ABILITY_ID::RESEARCH_ADEPTRESONATINGGLAIVES, UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL);
                    TryBuildUnit(ABILITY_ID::RESEARCH_PROTOSSSHIELDS, UNIT_TYPEID::PROTOSS_FORGE);
                    TryBuildUnit(ABILITY_ID::RESEARCH_INTERCEPTORGRAVITONCATAPULT, UNIT_TYPEID::PROTOSS_FLEETBEACON);
                }
                else {
                    TryBuildUnit(ABILITY_ID::RESEARCH_EXTENDEDTHERMALLANCE, UNIT_TYPEID::PROTOSS_ROBOTICSBAY);
                    TryBuildUnit(ABILITY_ID::RESEARCH_BLINK, UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL);
                    TryBuildUnit(ABILITY_ID::RESEARCH_CHARGE, UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL);
                    TryBuildUnit(ABILITY_ID::RESEARCH_PROTOSSGROUNDWEAPONS, UNIT_TYPEID::PROTOSS_FORGE);
                    TryBuildUnit(ABILITY_ID::RESEARCH_PROTOSSGROUNDARMOR, UNIT_TYPEID::PROTOSS_FORGE);
                }

            }
        }
    }
}

bool ProtossMultiplayerBot::TryWarpInUnit(ABILITY_ID ability_type_for_unit) {
    const ObservationInterface* observation = Observation();
    std::vector<PowerSource> power_sources = observation->GetPowerSources();
    Units warpgates = observation->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::PROTOSS_WARPGATE));

    if (power_sources.empty()) {
        return false;
    }

    const PowerSource& random_power_source = GetRandomEntry(power_sources);
    float radius = random_power_source.radius;
    float rx = GetRandomScalar();
    float ry = GetRandomScalar();
    Point2D build_location = Point2D(random_power_source.position.x + rx * radius, random_power_source.position.y + ry * radius);

    // If the warp location is walled off, don't warp there.
    // We check this to see if there is pathing from the build location to the center of the map
    if (Query()->PathingDistance(build_location, Point2D(game_info_.playable_max.x / 2, game_info_.playable_max.y / 2)) < .01f) {
        return false;
    }

    for (const auto& warpgate : warpgates) {
        if (warpgate.build_progress == 1) {
            AvailableAbilities abilities = Query()->GetAbilitiesForUnit(warpgate.tag);
            for (const auto& ability : abilities.abilities) {
                if (ability.ability_id == ability_type_for_unit) {
                    Actions()->UnitCommand(warpgate.tag, ability_type_for_unit, build_location);
                    return true;
                }
            }
        }
    }
    return false;
}

void ProtossMultiplayerBot::ConvertGateWayToWarpGate() {
    const ObservationInterface* observation = Observation();
    Units gateways = observation->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::PROTOSS_GATEWAY));

    if (warpgate_reasearched_) {
        for (const auto& gateway : gateways) {
            if (gateway.build_progress == 1) {
                Actions()->UnitCommand(gateway.tag, ABILITY_ID::MORPH_WARPGATE);
            }
        }
    }
}

bool ProtossMultiplayerBot::TryBuildStructureNearPylon(AbilityID ability_type_for_structure, UnitTypeID) {
    const ObservationInterface* observation = Observation();

    //Need to check to make sure its a pylon instead of a warp prism
    std::vector<PowerSource> power_sources = observation->GetPowerSources();
    if (power_sources.empty()) {
        return false;
    }

    const PowerSource& random_power_source = GetRandomEntry(power_sources);
    if (observation->GetUnit(random_power_source.tag) != nullptr) {
        if (observation->GetUnit(random_power_source.tag)->unit_type == UNIT_TYPEID::PROTOSS_WARPPRISM) {
            return false;
        }
    }
    else {
        return false;
    }
    float radius = random_power_source.radius;
    float rx = GetRandomScalar();
    float ry = GetRandomScalar();
    Point2D build_location = Point2D(random_power_source.position.x + rx * radius, random_power_source.position.y + ry * radius);
    return TryBuildStructure(ability_type_for_structure, UNIT_TYPEID::PROTOSS_PROBE, build_location);
}

bool ProtossMultiplayerBot::TryBuildPylon() {
    const ObservationInterface* observation = Observation();

    // If we are not supply capped, don't build a supply depot.
    if (observation->GetFoodUsed() < observation->GetFoodCap() - 6) {
        return false;
    }

    if (observation->GetMinerals() < 100) {
        return false;
    }

    //check to see if there is already on building
    Units units = observation->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::PROTOSS_PYLON));
    if (observation->GetFoodUsed() < 40) {
        for (const auto& unit : units) {
            if (unit.build_progress != 1) {
                    return false;
            }
        }
    }

    // Try and build a pylon. Find a random Probe and give it the order.
    float rx = GetRandomScalar();
    float ry = GetRandomScalar();
    Point2D build_location = Point2D(staging_location_.x + rx * 15, staging_location_.y + ry * 15);
    return TryBuildStructure(ABILITY_ID::BUILD_PYLON, UNIT_TYPEID::PROTOSS_PROBE, build_location);
}

//Separated per race due to gas timings
bool ProtossMultiplayerBot::TryBuildAssimilator() {
    const ObservationInterface* observation = Observation();
    Units bases = observation->GetUnits(Unit::Alliance::Self, IsTownHall());

    if (CountUnitType(observation, UNIT_TYPEID::PROTOSS_ASSIMILATOR) >= observation->GetUnits(Unit::Alliance::Self, IsTownHall()).size() * 2) {
        return false;
    }

    for (const auto& base : bases) {
        if (base.assigned_harvesters >= base.ideal_harvesters) {
            if (base.build_progress == 1) {
                if (TryBuildGas(ABILITY_ID::BUILD_ASSIMILATOR, UNIT_TYPEID::PROTOSS_PROBE, base.pos)) {
                    return true;
                }
            }
        }
    }
    return false;
}

// Same as above with expansion timings
bool ProtossMultiplayerBot::TryBuildExpansionNexus() {
    const ObservationInterface* observation = Observation();

    //Don't have more active bases than we can provide workers for
    if (GetExpectedWorkers(UNIT_TYPEID::PROTOSS_ASSIMILATOR) > max_worker_count_) {
        return false;
    }
    // If we have extra workers around, try and build another nexus.
    if (GetExpectedWorkers(UNIT_TYPEID::PROTOSS_ASSIMILATOR) < observation->GetFoodWorkers() - 16) {
        return TryExpand(ABILITY_ID::BUILD_NEXUS, UNIT_TYPEID::PROTOSS_PROBE);
    }
    //Only build another nexus if we are floating extra minerals
    if (observation->GetMinerals() > CountUnitType(observation, UNIT_TYPEID::PROTOSS_NEXUS) * 400) {
        return TryExpand(ABILITY_ID::BUILD_NEXUS, UNIT_TYPEID::PROTOSS_PROBE);
    }
    return false;
}

bool ProtossMultiplayerBot::TryBuildProbe() {
    const ObservationInterface* observation = Observation();
    Units bases = observation->GetUnits(Unit::Alliance::Self, IsTownHall());
    if (observation->GetFoodWorkers() >= max_worker_count_) {
        return false;
    }

    if (observation->GetFoodUsed() >= observation->GetFoodCap()) {
        return false;
    }

    if (observation->GetFoodWorkers() > GetExpectedWorkers(UNIT_TYPEID::PROTOSS_ASSIMILATOR)) {
        return false;
    }

    for (const auto& base : bases) {
        //if there is a base with less than ideal workers
        if (base.assigned_harvesters < base.ideal_harvesters && base.build_progress == 1) {
            if (observation->GetMinerals() >= 50) {
                return TryBuildUnit(ABILITY_ID::TRAIN_PROBE, UNIT_TYPEID::PROTOSS_NEXUS);
            }
        }
    }
    return false;
}

void ProtossMultiplayerBot::OnStep() {

    const ObservationInterface* observation = Observation();

    //Throttle some behavior that can wait to avoid duplicate orders.
    int frames_to_skip = 4;
    if (observation->GetFoodUsed() >= observation->GetFoodCap()) {
        frames_to_skip = 6;
    }

    if (observation->GetGameLoop() % frames_to_skip != 0) {
        return;
    }

    if (!nuke_detected) {
        ManageArmy();
    }
    else if (nuke_detected) {
        if (nuke_detected_frame + 400 < observation->GetGameLoop()) {
            nuke_detected = false;
        }
        Units units = observation->GetUnits(Unit::Self, IsArmy(observation));
        for (const auto& unit : units) {
            RetreatWithUnit(unit, startLocation_);
        }
    }

    ConvertGateWayToWarpGate();

    ManageWorkers(UNIT_TYPEID::PROTOSS_PROBE, ABILITY_ID::HARVEST_GATHER, UNIT_TYPEID::PROTOSS_ASSIMILATOR);

    ManageUpgrades();

    BuildOrder();

    if (TryBuildPylon()) {
        return;
    }

    if (TryBuildAssimilator()) {
        return;
    }

    if (TryBuildProbe()) {
        return;
    }

    if (TryBuildArmy()) {
        return;
    }

    if (TryBuildExpansionNexus()) {
        return;
    }
}

void ProtossMultiplayerBot::OnGameEnd() {
    std::cout << "Game Ended for: " << std::to_string(Control()->Proto().GetAssignedPort()) << std::endl;
}

void ProtossMultiplayerBot::OnUnitIdle(const Unit& unit) {
    switch (unit.unit_type.ToType()) {
        case UNIT_TYPEID::PROTOSS_PROBE: {
            MineIdleWorkers(unit.tag, ABILITY_ID::HARVEST_GATHER, UNIT_TYPEID::PROTOSS_ASSIMILATOR);
            break;
        }
        case UNIT_TYPEID::PROTOSS_CYBERNETICSCORE: {
            const ObservationInterface* observation = Observation();
            Units nexus = observation->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::PROTOSS_NEXUS));

            if (!warpgate_reasearched_) {
                Actions()->UnitCommand(unit.tag, ABILITY_ID::RESEARCH_WARPGATE);
                if (!nexus.empty()) {
                    Actions()->UnitCommand(nexus.front().tag, ABILITY_ID::EFFECT_TIMEWARP, unit.tag);
                }
            }
        }
        default:
            break;
    }
}

void ProtossMultiplayerBot::OnUpgradeCompleted(UpgradeID upgrade) {
    switch (upgrade.ToType()) {
        case UPGRADE_ID::WARPGATERESEARCH: {
            warpgate_reasearched_ = true;
        }
        case UPGRADE_ID::BLINKTECH: {
            blink_reasearched_ = true;
        }
        default:
            break;
    }
}

bool ZergMultiplayerBot::TryBuildDrone() {
    const ObservationInterface* observation = Observation();
    size_t larva_count = CountUnitType(observation, UNIT_TYPEID::ZERG_LARVA);
    Units bases = observation->GetUnits(Unit::Alliance::Self, IsTownHall());
    size_t worker_count = CountUnitType(observation, UNIT_TYPEID::ZERG_DRONE);
    Units eggs = observation->GetUnits(Unit::Self, IsUnit(UNIT_TYPEID::ZERG_EGG));
    for (const auto& egg : eggs) {
        if (!egg.orders.empty()) {
            if (egg.orders.front().ability_id == ABILITY_ID::TRAIN_DRONE) {
                worker_count++;
            }
        }
    }
    if (worker_count >= max_worker_count_) {
        return false;
    }

    if (worker_count > GetExpectedWorkers(UNIT_TYPEID::ZERG_EXTRACTOR)) {
        return false;
    }

    if (observation->GetFoodUsed() >= observation->GetFoodCap()) {
        return false;
    }

    for (const auto& base : bases) {
        //if there is a base with less than ideal workers
        if (base.assigned_harvesters < base.ideal_harvesters && base.build_progress == 1) {
            if (observation->GetMinerals() >= 50 && larva_count > 0) {
                Tag mineral;
                FindNearestMineralPatch(base.pos, mineral);
                return TryBuildUnit(ABILITY_ID::TRAIN_DRONE, UNIT_TYPEID::ZERG_LARVA);
            }
        }
    }
    return false;
}

bool ZergMultiplayerBot::TryBuildOnCreep(AbilityID ability_type_for_structure, UnitTypeID unit_type) {
    float rx = GetRandomScalar();
    float ry = GetRandomScalar();
    const ObservationInterface* observation = Observation();
    Point2D build_location = Point2D(startLocation_.x + rx * 15, startLocation_.y + ry * 15);

    if (observation->HasCreep(build_location)) {
        return TryBuildStructure(ability_type_for_structure, unit_type, build_location);
    }
    return false;
}

void ZergMultiplayerBot::BuildOrder() {
    const ObservationInterface* observation = Observation();
    bool hive_tech = CountUnitType(observation, UNIT_TYPEID::ZERG_HIVE) > 0;
    bool lair_tech = CountUnitType(observation, UNIT_TYPEID::ZERG_LAIR) > 0 || hive_tech;
    size_t base_count = observation->GetUnits(Unit::Self, IsTownHall()).size();
    size_t evolution_chanber_target = 1;
    size_t morphing_lair = 0;
    size_t morphing_hive = 0;
    Units hatcherys = observation->GetUnits(Unit::Self, IsUnit(UNIT_TYPEID::ZERG_HATCHERY));
    Units lairs = observation->GetUnits(Unit::Self, IsUnit(UNIT_TYPEID::ZERG_LAIR));
    for (const auto& hatchery : hatcherys) {
        if (!hatchery.orders.empty()) {
            if (hatchery.orders.front().ability_id == ABILITY_ID::MORPH_LAIR) {
                ++morphing_lair;
            }
        }
    }
    for (const auto& lair : lairs) {
        if (!lair.orders.empty()) {
            if (lair.orders.front().ability_id == ABILITY_ID::MORPH_HIVE) {
                ++morphing_hive;
            }
        }
    }

    if (!mutalisk_build_) {
        evolution_chanber_target++;
    }
    //Priority to spawning pool
    if (CountUnitType(observation, UNIT_TYPEID::ZERG_SPAWNINGPOOL) < 1) {
        TryBuildOnCreep(ABILITY_ID::BUILD_SPAWNINGPOOL, UNIT_TYPEID::ZERG_DRONE);
    }
    else {
        if (base_count < 1) {
            TryBuildExpansionHatch();
            return;
        }

        if (CountUnitType(observation, UNIT_TYPEID::ZERG_EVOLUTIONCHAMBER) < evolution_chanber_target) {
            TryBuildOnCreep(ABILITY_ID::BUILD_EVOLUTIONCHAMBER, UNIT_TYPEID::ZERG_DRONE);
        }

        if (!mutalisk_build_ && CountUnitType(observation, UNIT_TYPEID::ZERG_ROACHWARREN) < 1) {
            TryBuildOnCreep(ABILITY_ID::BUILD_ROACHWARREN, UNIT_TYPEID::ZERG_DRONE);
        }

        if (!lair_tech) {
            if (CountUnitType(observation, UNIT_TYPEID::ZERG_HIVE) + CountUnitType(observation, UNIT_TYPEID::ZERG_LAIR) < 1 && CountUnitType(observation,UNIT_TYPEID::ZERG_QUEEN) > 0) {
                TryBuildUnit(ABILITY_ID::MORPH_LAIR, UNIT_TYPEID::ZERG_HATCHERY);
            }
        }
        else {
            if (!mutalisk_build_) {
                if (CountUnitType(observation, UNIT_TYPEID::ZERG_HYDRALISKDEN) + CountUnitType(observation, UNIT_TYPEID::ZERG_LURKERDENMP) < 1) {
                    TryBuildOnCreep(ABILITY_ID::BUILD_HYDRALISKDEN, UNIT_TYPEID::ZERG_DRONE);
                }
                if (CountUnitType(observation, UNIT_TYPEID::ZERG_HYDRALISKDEN) > 0) {
                    TryBuildUnit(ABILITY_ID::MORPH_LURKERDEN, UNIT_TYPEID::ZERG_HYDRALISKDEN);
                }
            }
            else {
                if (CountUnitType(observation, UNIT_TYPEID::ZERG_SPIRE) + CountUnitType(observation, UNIT_TYPEID::ZERG_GREATERSPIRE) < 1) {
                    TryBuildOnCreep(ABILITY_ID::BUILD_SPIRE, UNIT_TYPEID::ZERG_DRONE);
                }
            }

            if (base_count < 3) {
                TryBuildExpansionHatch();
                return;
            }
            if (CountUnitType(observation, UNIT_TYPEID::ZERG_INFESTATIONPIT) > 0 && CountUnitType(observation,UNIT_TYPEID::ZERG_HIVE) < 1) {
                TryBuildUnit(ABILITY_ID::MORPH_HIVE, UNIT_TYPEID::ZERG_LAIR);
                return;
            }

            if (CountUnitType(observation, UNIT_TYPEID::ZERG_BANELINGNEST) < 1) {
                TryBuildOnCreep(ABILITY_ID::BUILD_BANELINGNEST, UNIT_TYPEID::ZERG_DRONE);
            }

            if (observation->GetUnits(Unit::Self, IsTownHall()).size() > 2) {
                if (CountUnitType(observation, UNIT_TYPEID::ZERG_INFESTATIONPIT) < 1) {
                    TryBuildOnCreep(ABILITY_ID::BUILD_INFESTATIONPIT, UNIT_TYPEID::ZERG_DRONE);
                }
            }

        }

        if (hive_tech) {

            if (!mutalisk_build_ && CountUnitType(observation, UNIT_TYPEID::ZERG_ULTRALISKCAVERN) < 1) {
                TryBuildOnCreep(ABILITY_ID::BUILD_ULTRALISKCAVERN, UNIT_TYPEID::ZERG_DRONE);
            }

            if (CountUnitType(observation, UNIT_TYPEID::ZERG_GREATERSPIRE) < 1) {
                TryBuildUnit(ABILITY_ID::MORPH_GREATERSPIRE, UNIT_TYPEID::ZERG_SPIRE);
            }
        }

    }
}

void ZergMultiplayerBot::ManageArmy() {
    const ObservationInterface* observation = Observation();

    Units enemy_units = observation->GetUnits(Unit::Alliance::Enemy);
    Units army = observation->GetUnits(Unit::Alliance::Self, IsArmy(observation));
    int wait_til_supply = 100;

    if (enemy_units.empty() && observation->GetFoodArmy() < wait_til_supply) {
        for (const auto& unit : army) {
            switch (unit.unit_type.ToType()) {
                case(UNIT_TYPEID::ZERG_LURKERMPBURROWED): {
                    Actions()->UnitCommand(unit.tag, ABILITY_ID::BURROWUP);
                }
                default:
                    RetreatWithUnit(unit, staging_location_);
                    break;
            }
        }
    }
    else if (!enemy_units.empty()) {
        for (const auto& unit : army) {
            switch (unit.unit_type.ToType()) {
                case(UNIT_TYPEID::ZERG_CORRUPTOR) : {
                    Tag closest_unit;
                    float distance = std::numeric_limits<float>::max();
                    for (const auto& u : enemy_units) {
                        float d = Distance2D(u.pos, unit.pos);
                        if (d < distance) {
                            distance = d;
                            closest_unit = u.tag;
                        }
                    }
                    const Unit* enemy_unit = observation->GetUnit(closest_unit);

                    auto attributes = observation->GetUnitTypeData().at(enemy_unit->unit_type).attributes;
                    for (const auto& attribute : attributes) {
                        if (attribute == Attribute::Structure) {
                            Actions()->UnitCommand(unit.tag, ABILITY_ID::EFFECT_CAUSTICSPRAY, enemy_unit->tag);
                        }
                    }
                    if (!unit.orders.empty()) {
                        if (unit.orders.front().ability_id == ABILITY_ID::EFFECT_CAUSTICSPRAY) {
                            break;
                        }
                    }
                    AttackWithUnit(unit, observation);
                    break;
               }
                case(UNIT_TYPEID::ZERG_OVERSEER): {
                    Actions()->UnitCommand(unit.tag, ABILITY_ID::ATTACK, enemy_units.front());
                    break;
                }
                case(UNIT_TYPEID::ZERG_RAVAGER): {
                    Point2D closest_unit;
                    float distance = std::numeric_limits<float>::max();
                    for (const auto& u : enemy_units) {
                        float d = Distance2D(u.pos, unit.pos);
                        if (d < distance) {
                            distance = d;
                            closest_unit = u.pos;
                        }
                    }
                    Actions()->UnitCommand(unit.tag, ABILITY_ID::EFFECT_CORROSIVEBILE, closest_unit);
                    AttackWithUnit(unit, observation);
                }
                case(UNIT_TYPEID::ZERG_LURKERMP): {
                    Point2D closest_unit;
                    float distance = std::numeric_limits<float>::max();
                    for (const auto& u : enemy_units) {
                        float d = Distance2D(u.pos, unit.pos);
                        if (d < distance) {
                            distance = d;
                            closest_unit = u.pos;
                        }
                    }
                    if (distance < 7) {
                        Actions()->UnitCommand(unit.tag, ABILITY_ID::BURROWDOWN);
                    }
                    else {
                        Actions()->UnitCommand(unit.tag, ABILITY_ID::ATTACK, closest_unit);
                    }
                    break;
                }
                case(UNIT_TYPEID::ZERG_LURKERMPBURROWED): {
                    float distance = std::numeric_limits<float>::max();
                    for (const auto& u : enemy_units) {
                        float d = Distance2D(u.pos, unit.pos);
                        if (d < distance) {
                            distance = d;
                        }
                    }
                    if (distance > 9) {
                        Actions()->UnitCommand(unit.tag, ABILITY_ID::BURROWUP);
                    }
                    break;
                }
                case(UNIT_TYPEID::ZERG_SWARMHOSTMP): {
                    Point2D closest_unit;
                    float distance = std::numeric_limits<float>::max();
                    for (const auto& u : enemy_units) {
                        float d = Distance2D(u.pos, unit.pos);
                        if (d < distance) {
                            distance = d;
                            closest_unit = u.pos;
                        }
                    }
                    if (distance < 15) {
                        const auto abilities = Query()->GetAbilitiesForUnit(unit.tag).abilities;
                        bool ability_available = false;
                        for (const auto& ability : abilities) {
                            if (ability.ability_id == ABILITY_ID::EFFECT_SPAWNLOCUSTS) {
                                ability_available = true;
                            }
                        }
                        if (ability_available) {
                            Actions()->UnitCommand(unit.tag, ABILITY_ID::EFFECT_SPAWNLOCUSTS, closest_unit);
                        }
                        else {
                            RetreatWithUnit(unit, staging_location_);
                        }
                    }
                    else {
                        Actions()->UnitCommand(unit.tag, ABILITY_ID::ATTACK, closest_unit);
                    }
                    break;
                }
                case(UNIT_TYPEID::ZERG_INFESTOR): {
                    Point2D closest_unit;
                    float distance = std::numeric_limits<float>::max();
                    for (const auto& u : enemy_units) {
                        float d = Distance2D(u.pos, unit.pos);
                        if (d < distance) {
                            distance = d;
                            closest_unit = u.pos;
                        }
                    }
                    if (distance < 9) {
                        const auto abilities = Query()->GetAbilitiesForUnit(unit.tag).abilities;
                        if (unit.energy > 75) {
                            Actions()->UnitCommand(unit.tag, ABILITY_ID::EFFECT_FUNGALGROWTH, closest_unit);
                        }
                        else {
                            RetreatWithUnit(unit, staging_location_);
                        }
                    }
                    else {
                        Actions()->UnitCommand(unit.tag, ABILITY_ID::ATTACK, closest_unit);
                    }
                    break;
                }
                case(UNIT_TYPEID::ZERG_VIPER): {
                    Tag closest_unit;
                    bool is_flying = false;
                    float distance = std::numeric_limits<float>::max();
                    for (const auto& u : enemy_units) {
                        auto attributes = observation->GetUnitTypeData().at(u.unit_type).attributes;
                        bool is_structure = false;
                        for (const auto& attribute : attributes) {
                            if (attribute == Attribute::Structure) {
                                is_structure = true;
                            }
                        }
                        if (is_structure) {
                            continue;
                        }
                        float d = Distance2D(u.pos, unit.pos);
                        if (d < distance) {
                            distance = d;
                            closest_unit = u.tag;
                            is_flying = u.is_flying;
                        }
                    }
                    if (distance < 10) {
                        if (is_flying && unit.energy > 124) {
                            Actions()->UnitCommand(unit.tag, ABILITY_ID::EFFECT_PARASITICBOMB, closest_unit);
                        }
                        else if (!is_flying && unit.energy > 100) {
                            Point2D target = observation->GetUnit(closest_unit)->pos;
                            Actions()->UnitCommand(unit.tag, ABILITY_ID::EFFECT_BLINDINGCLOUD, target);
                        }
                        else {
                            RetreatWithUnit(unit, startLocation_);
                        }

                    }
                    else {
                        if (unit.energy > 124) {
                            Actions()->UnitCommand(unit.tag, ABILITY_ID::ATTACK, enemy_units.front());
                        }
                        else {
                            if (!unit.orders.empty()) {
                                if (unit.orders.front().ability_id != ABILITY_ID::EFFECT_VIPERCONSUME) {
                                    Units extractors = observation->GetUnits(Unit::Self, IsUnit(UNIT_TYPEID::ZERG_EXTRACTOR));
                                    for (const auto& extractor : extractors) {
                                        if (extractor.health > 200) {
                                            Actions()->UnitCommand(unit.tag, ABILITY_ID::EFFECT_VIPERCONSUME, extractor.tag);
                                        }
                                        else {
                                            continue;
                                        }
                                    }
                                }
                                else {
                                    if (observation->GetUnit(unit.orders.front().target_unit_tag)->health < 100) {
                                        Actions()->UnitCommand(unit.tag, ABILITY_ID::STOP);
                                    }
                                }
                            }
                            else {
                                Actions()->UnitCommand(unit.tag, ABILITY_ID::ATTACK, closest_unit);
                            }
                        }
                    }
                    break;
                }
                default: {
                    AttackWithUnit(unit, observation);
                }
           }
        }
    }
    else {
        for (const auto& unit : army) {
            switch (unit.unit_type.ToType()){
                case(UNIT_TYPEID::ZERG_LURKERMPBURROWED): {
                    Actions()->UnitCommand(unit.tag, ABILITY_ID::BURROWUP);
                }
                default:
                    ScoutWithUnit(unit, observation);
                    break;
            }
        }
    }
}

void ZergMultiplayerBot::BuildArmy() {
    const ObservationInterface* observation = Observation();
    size_t larva_count = CountUnitType(observation, UNIT_TYPEID::ZERG_LARVA);
    size_t base_count = observation->GetUnits(Unit::Self, IsTownHall()).size();

    size_t queen_Count = CountUnitTypeTotal(observation, UNIT_TYPEID::ZERG_QUEEN, UNIT_TYPEID::ZERG_HATCHERY, ABILITY_ID::TRAIN_QUEEN);
    queen_Count += CountUnitTypeBuilding(observation, UNIT_TYPEID::ZERG_LAIR, ABILITY_ID::TRAIN_QUEEN);
    queen_Count += CountUnitTypeBuilding(observation, UNIT_TYPEID::ZERG_HIVE, ABILITY_ID::TRAIN_QUEEN);
    size_t hydralisk_count = CountUnitTypeTotal(observation, UNIT_TYPEID::ZERG_HYDRALISK, UNIT_TYPEID::ZERG_EGG, ABILITY_ID::TRAIN_HYDRALISK);
    size_t roach_count = CountUnitTypeTotal(observation, UNIT_TYPEID::ZERG_ROACH, UNIT_TYPEID::ZERG_EGG, ABILITY_ID::TRAIN_ROACH);
    size_t corruptor_count = CountUnitTypeTotal(observation, UNIT_TYPEID::ZERG_CORRUPTOR, UNIT_TYPEID::ZERG_EGG, ABILITY_ID::TRAIN_CORRUPTOR);
    size_t swarmhost_count = CountUnitTypeTotal(observation, UNIT_TYPEID::ZERG_SWARMHOSTMP, UNIT_TYPEID::ZERG_EGG, ABILITY_ID::TRAIN_SWARMHOST);
    size_t viper_count = CountUnitTypeTotal(observation, UNIT_TYPEID::ZERG_VIPER, UNIT_TYPEID::ZERG_EGG, ABILITY_ID::TRAIN_VIPER);
    size_t ultralisk_count = CountUnitTypeTotal(observation, UNIT_TYPEID::ZERG_ULTRALISK, UNIT_TYPEID::ZERG_EGG, ABILITY_ID::TRAIN_ULTRALISK);
    size_t infestor_count = CountUnitTypeTotal(observation, UNIT_TYPEID::ZERG_INFESTOR, UNIT_TYPEID::ZERG_EGG, ABILITY_ID::TRAIN_INFESTOR);

    if (queen_Count < base_count && CountUnitType(observation, UNIT_TYPEID::ZERG_SPAWNINGPOOL) > 0) {
        if (observation->GetMinerals() >= 150) {
            if (!TryBuildUnit(ABILITY_ID::TRAIN_QUEEN, UNIT_TYPEID::ZERG_HATCHERY)) {
                if (!TryBuildUnit(ABILITY_ID::TRAIN_QUEEN, UNIT_TYPEID::ZERG_LAIR)) {
                    TryBuildUnit(ABILITY_ID::TRAIN_QUEEN, UNIT_TYPEID::ZERG_HIVE);
                }
            }
        }
    }
    if (CountUnitType(observation, UNIT_TYPEID::ZERG_OVERSEER) + CountUnitType(observation, UNIT_TYPEID::ZERG_OVERLORDCOCOON) < 1) {
        TryBuildUnit(ABILITY_ID::MORPH_OVERSEER, UNIT_TYPEID::ZERG_OVERLORD);
    }

    if (larva_count > 0) {
        if (viper_count < 2) {
            if (TryBuildUnit(ABILITY_ID::TRAIN_VIPER, UNIT_TYPEID::ZERG_LARVA)) {
                --larva_count;
            }
        }
        if (CountUnitType(observation, UNIT_TYPEID::ZERG_ULTRALISKCAVERN) > 0 && !mutalisk_build_ && ultralisk_count < 4) {
            if (TryBuildUnit(ABILITY_ID::TRAIN_ULTRALISK, UNIT_TYPEID::ZERG_LARVA)) {
                --larva_count;
            }
            //Try to build at least one ultralisk
            if (ultralisk_count < 1) {
                return;
            }
        }
    }

    if (!mutalisk_build_) {
        if (CountUnitType(observation, UNIT_TYPEID::ZERG_RAVAGER) + CountUnitType(observation, UNIT_TYPEID::ZERG_RAVAGERCOCOON) < 3) {
            TryBuildUnit(ABILITY_ID::MORPH_RAVAGER, UNIT_TYPEID::ZERG_ROACH);
        }
        if (CountUnitType(observation, UNIT_TYPEID::ZERG_LURKERMP) + CountUnitType(observation, UNIT_TYPEID::ZERG_LURKERMPEGG) + CountUnitType(observation, UNIT_TYPEID::ZERG_LURKERMPBURROWED) < 6) {
            TryBuildUnit(ABILITY_ID::MORPH_LURKER, UNIT_TYPEID::ZERG_HYDRALISK);
        }
        if (larva_count > 0) {
            if (CountUnitType(observation, UNIT_TYPEID::ZERG_HYDRALISKDEN) + CountUnitType(observation, UNIT_TYPEID::ZERG_LURKERDENMP) > 0 && hydralisk_count < 15) {
                if (TryBuildUnit(ABILITY_ID::TRAIN_HYDRALISK, UNIT_TYPEID::ZERG_LARVA)) {
                    --larva_count;
                }
            }
        }

        if (larva_count > 0) {
            if (swarmhost_count < 1) {
                if (TryBuildUnit(ABILITY_ID::TRAIN_SWARMHOST, UNIT_TYPEID::ZERG_LARVA)) {
                    --larva_count;
                }
            }
        }

        if (larva_count > 0) {
            if (infestor_count < 1) {
                if (TryBuildUnit(ABILITY_ID::TRAIN_INFESTOR, UNIT_TYPEID::ZERG_LARVA)) {
                    --larva_count;
                }
            }
        }
    }
    else {
        if (larva_count > 0) {
            if (CountUnitType(observation, UNIT_TYPEID::ZERG_SPIRE) + CountUnitType(observation, UNIT_TYPEID::ZERG_GREATERSPIRE) > 0) {
                if (corruptor_count < 7) {
                    if (TryBuildUnit(ABILITY_ID::TRAIN_CORRUPTOR, UNIT_TYPEID::ZERG_LARVA)) {
                        --larva_count;
                    }
                }
                if (TryBuildUnit(ABILITY_ID::TRAIN_MUTALISK, UNIT_TYPEID::ZERG_LARVA)) {
                    --larva_count;
                }
            }
        }
    }

    if (larva_count > 0) {
        if (++viper_count < 1) {
            if (TryBuildUnit(ABILITY_ID::TRAIN_VIPER, UNIT_TYPEID::ZERG_LARVA)) {
                --larva_count;
            }
        }
        if (CountUnitType(observation, UNIT_TYPEID::ZERG_ULTRALISKCAVERN) > 0 && !mutalisk_build_ && ultralisk_count < 4) {
            if (TryBuildUnit(ABILITY_ID::TRAIN_ULTRALISK, UNIT_TYPEID::ZERG_LARVA)) {
                --larva_count;
            }
            //Try to build at least one ultralisk
            if (ultralisk_count < 1) {
                return;
            }
        }
    }
    if (CountUnitType(observation, UNIT_TYPEID::ZERG_GREATERSPIRE) > 0) {
        if(CountUnitType(observation, UNIT_TYPEID::ZERG_BROODLORD) + CountUnitType(observation,UNIT_TYPEID::ZERG_BROODLORDCOCOON) < 4)
        TryBuildUnit(ABILITY_ID::MORPH_BROODLORD, UNIT_TYPEID::ZERG_CORRUPTOR);
    }

    if (!mutalisk_build_ && larva_count > 0) {
        if (roach_count < 10 && CountUnitType(observation, UNIT_TYPEID::ZERG_ROACHWARREN) > 0) {
            if (TryBuildUnit(ABILITY_ID::TRAIN_ROACH, UNIT_TYPEID::ZERG_LARVA)) {
                --larva_count;
            }
        }
    }
    size_t baneling_count = CountUnitType(observation, UNIT_TYPEID::ZERG_BANELING) + CountUnitType(observation, UNIT_TYPEID::ZERG_BANELINGCOCOON);
    if (larva_count > 0) {
        if (CountUnitType(observation, UNIT_TYPEID::ZERG_ZERGLING) < 20 && CountUnitType(observation, UNIT_TYPEID::ZERG_SPAWNINGPOOL) > 0) {
            if (TryBuildUnit(ABILITY_ID::TRAIN_ZERGLING, UNIT_TYPEID::ZERG_LARVA)) {
                --larva_count;
            }
        }
    }

    size_t baneling_target = 5;
    if (mutalisk_build_) {
        baneling_target = baneling_target * 2;
    }
    if (baneling_count < baneling_target) {
        TryBuildUnit(ABILITY_ID::TRAIN_BANELING, UNIT_TYPEID::ZERG_ZERGLING);
    }

}

void ZergMultiplayerBot::ManageUpgrades() {
    const ObservationInterface* observation = Observation();
    auto upgrades = observation->GetUpgrades();
    size_t base_count = observation->GetUnits(Unit::Alliance::Self, IsTownHall()).size();
    bool hive_tech = CountUnitType(observation, UNIT_TYPEID::ZERG_HIVE) > 0;
    bool lair_tech = CountUnitType(observation, UNIT_TYPEID::ZERG_LAIR) > 0 || hive_tech;

    if (upgrades.empty()) {
        TryBuildUnit(ABILITY_ID::RESEARCH_ZERGLINGMETABOLICBOOST, UNIT_TYPEID::ZERG_SPAWNINGPOOL);
    }
    else {
        for (const auto& upgrade : upgrades) {
            if (mutalisk_build_) {
                if (upgrade == UPGRADE_ID::ZERGFLYERWEAPONSLEVEL1 && base_count > 3) {
                    TryBuildUnit(ABILITY_ID::RESEARCH_ZERGFLYERATTACK, UNIT_TYPEID::ZERG_SPIRE);
                    TryBuildUnit(ABILITY_ID::RESEARCH_ZERGFLYERATTACK, UNIT_TYPEID::ZERG_GREATERSPIRE);
                }
                else if (upgrade == UPGRADE_ID::ZERGFLYERARMORSLEVEL1 && base_count > 3) {
                    TryBuildUnit(ABILITY_ID::RESEARCH_ZERGFLYERARMOR, UNIT_TYPEID::ZERG_SPIRE);
                    TryBuildUnit(ABILITY_ID::RESEARCH_ZERGFLYERARMOR, UNIT_TYPEID::ZERG_GREATERSPIRE);
                }
                else if (upgrade == UPGRADE_ID::ZERGFLYERWEAPONSLEVEL2 && base_count > 4) {
                    TryBuildUnit(ABILITY_ID::RESEARCH_ZERGFLYERATTACK, UNIT_TYPEID::ZERG_SPIRE);
                    TryBuildUnit(ABILITY_ID::RESEARCH_ZERGFLYERATTACK, UNIT_TYPEID::ZERG_GREATERSPIRE);
                }
                else if (upgrade == UPGRADE_ID::ZERGFLYERARMORSLEVEL2 && base_count > 4) {
                    TryBuildUnit(ABILITY_ID::RESEARCH_ZERGFLYERARMOR, UNIT_TYPEID::ZERG_SPIRE);
                    TryBuildUnit(ABILITY_ID::RESEARCH_ZERGFLYERARMOR, UNIT_TYPEID::ZERG_GREATERSPIRE);
                }
                else {
                    TryBuildUnit(ABILITY_ID::RESEARCH_ZERGFLYERATTACK, UNIT_TYPEID::ZERG_SPIRE);
                    TryBuildUnit(ABILITY_ID::RESEARCH_ZERGFLYERATTACK, UNIT_TYPEID::ZERG_GREATERSPIRE);
                    TryBuildUnit(ABILITY_ID::RESEARCH_ZERGFLYERARMOR, UNIT_TYPEID::ZERG_SPIRE);
                    TryBuildUnit(ABILITY_ID::RESEARCH_ZERGFLYERARMOR, UNIT_TYPEID::ZERG_GREATERSPIRE);
                    TryBuildUnit(ABILITY_ID::RESEARCH_ZERGGROUNDARMOR, UNIT_TYPEID::ZERG_EVOLUTIONCHAMBER);
                    TryBuildUnit(ABILITY_ID::RESEARCH_ZERGMELEEWEAPONS, UNIT_TYPEID::ZERG_EVOLUTIONCHAMBER);
                }
            }//Not Mutalisk build only
            else {
                if (upgrade == UPGRADE_ID::ZERGMISSILEWEAPONSLEVEL1 && base_count > 3) {
                    TryBuildUnit(ABILITY_ID::RESEARCH_ZERGMISSILEWEAPONS, UNIT_TYPEID::ZERG_EVOLUTIONCHAMBER);
                }
                else if (upgrade == UPGRADE_ID::ZERGMISSILEWEAPONSLEVEL2 && base_count > 4) {
                    TryBuildUnit(ABILITY_ID::RESEARCH_ZERGMISSILEWEAPONS, UNIT_TYPEID::ZERG_EVOLUTIONCHAMBER);
                }
                if (upgrade == UPGRADE_ID::ZERGGROUNDARMORSLEVEL1 && base_count > 3) {
                    TryBuildUnit(ABILITY_ID::RESEARCH_ZERGGROUNDARMOR, UNIT_TYPEID::ZERG_EVOLUTIONCHAMBER);
                }
                else if (upgrade == UPGRADE_ID::ZERGGROUNDARMORSLEVEL2 && base_count > 4) {
                    TryBuildUnit(ABILITY_ID::RESEARCH_ZERGGROUNDARMOR, UNIT_TYPEID::ZERG_EVOLUTIONCHAMBER);
                }

                if (hive_tech) {
                    TryBuildUnit(ABILITY_ID::RESEARCH_CHITINOUSPLATING, UNIT_TYPEID::ZERG_ULTRALISKCAVERN);
                }

                if (lair_tech) {
                    TryBuildUnit(ABILITY_ID::RESEARCH_ZERGMISSILEWEAPONS, UNIT_TYPEID::ZERG_EVOLUTIONCHAMBER);
                    TryBuildUnit(ABILITY_ID::RESEARCH_ZERGGROUNDARMOR, UNIT_TYPEID::ZERG_EVOLUTIONCHAMBER);
                    TryBuildUnit(ABILITY_ID::RESEARCH_CENTRIFUGALHOOKS, UNIT_TYPEID::ZERG_BANELINGNEST);
                    TryBuildUnit(ABILITY_ID::RESEARCH_MUSCULARAUGMENTS, UNIT_TYPEID::ZERG_HYDRALISKDEN);
                    TryBuildUnit(ABILITY_ID::RESEARCH_MUSCULARAUGMENTS, UNIT_TYPEID::ZERG_LURKERDENMP);
                    TryBuildUnit(ABILITY_ID::RESEARCH_GLIALREGENERATION, UNIT_TYPEID::ZERG_ROACHWARREN);
                }
            }
            //research regardless of build
            if (hive_tech) {
                TryBuildUnit(ABILITY_ID::RESEARCH_ZERGLINGADRENALGLANDS, UNIT_TYPEID::ZERG_SPAWNINGPOOL);
            }
            else if (lair_tech) {
                TryBuildUnit(ABILITY_ID::RESEARCH_PNEUMATIZEDCARAPACE, UNIT_TYPEID::ZERG_HIVE);
                TryBuildUnit(ABILITY_ID::RESEARCH_PNEUMATIZEDCARAPACE, UNIT_TYPEID::ZERG_LAIR);
                TryBuildUnit(ABILITY_ID::RESEARCH_PNEUMATIZEDCARAPACE, UNIT_TYPEID::ZERG_HATCHERY);
            }
        }
    }
}

bool ZergMultiplayerBot::TryBuildOverlord() {
    const ObservationInterface* observation = Observation();
    size_t larva_count = CountUnitType(observation, UNIT_TYPEID::ZERG_LARVA);
    if (observation->GetFoodCap() == 200) {
        return false;
    }
    if (observation->GetFoodUsed() < observation->GetFoodCap() - 4) {
        return false;
    }
    if (observation->GetMinerals() < 100) {
        return false;
    }

    //Slow overlord development in the beginning
    if (observation->GetFoodUsed() < 30) {
        Units units = observation->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::ZERG_EGG));
        for (const auto& unit : units) {
            if (unit.orders.empty()) {
                return false;
            }
            if (unit.orders.front().ability_id == ABILITY_ID::TRAIN_OVERLORD) {
                return false;
            }
        }
    }
    if (larva_count > 0) {
        return TryBuildUnit(ABILITY_ID::TRAIN_OVERLORD, UNIT_TYPEID::ZERG_LARVA);
    }
    return false;
}

void ZergMultiplayerBot::TryInjectLarva() {
    const ObservationInterface* observation = Observation();
    Units queens = observation->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::ZERG_QUEEN));
    Units hatcheries = observation->GetUnits(Unit::Alliance::Self,IsTownHall());

    //if we don't have queens or hatcheries don't do anything
    if (queens.empty() || hatcheries.empty())
        return;

    for (size_t i = 0; i < queens.size(); ++i) {
        for (size_t j = 0; j < hatcheries.size(); ++j) {

            //if hatchery isn't complete ignore it
            if (hatcheries.at(j).build_progress != 1) {
                continue;
            }
            else {

                //Inject larva and move onto next available queen
                if (i < queens.size()) {
                    if (queens.at(i).energy >= 25 && queens.at(i).orders.empty()) {
                        Actions()->UnitCommand(queens.at(i).tag, ABILITY_ID::EFFECT_INJECTLARVA, hatcheries.at(j).tag);
                    }
                    ++i;
                }
            }
        }
    }
}

bool ZergMultiplayerBot::TryBuildExpansionHatch() {
    const ObservationInterface* observation = Observation();

    //Don't have more active bases than we can provide workers for
    if (GetExpectedWorkers(UNIT_TYPEID::ZERG_EXTRACTOR) > max_worker_count_) {
        return false;
    }
    // If we have extra workers around, try and build another Hatch.
    if (GetExpectedWorkers(UNIT_TYPEID::ZERG_EXTRACTOR) < observation->GetFoodWorkers() - 10) {
        return TryExpand(ABILITY_ID::BUILD_HATCHERY, UNIT_TYPEID::ZERG_DRONE);
    }
    //Only build another Hatch if we are floating extra minerals
    if (observation->GetMinerals() > std::min<size_t>((CountUnitType(observation, UNIT_TYPEID::ZERG_HATCHERY) * 300), 1200)) {
        return TryExpand(ABILITY_ID::BUILD_HATCHERY, UNIT_TYPEID::ZERG_DRONE);
    }
    return false;
}

bool ZergMultiplayerBot::BuildExtractor() {
    const ObservationInterface* observation = Observation();
    Units bases = observation->GetUnits(Unit::Alliance::Self, IsTownHall());

    if (CountUnitType(observation, UNIT_TYPEID::ZERG_EXTRACTOR) >= observation->GetUnits(Unit::Alliance::Self, IsTownHall()).size() * 2) {
        return false;
    }

    for (const auto& base : bases) {
        if (base.assigned_harvesters >= base.ideal_harvesters) {
            if (base.build_progress == 1) {
                if (TryBuildGas(ABILITY_ID::BUILD_EXTRACTOR, UNIT_TYPEID::ZERG_DRONE, base.pos)) {
                    return true;
                }
            }
        }
    }
    return false;
}

void ZergMultiplayerBot::OnStep() {

    const ObservationInterface* observation = Observation();
    Units base = observation->GetUnits(Unit::Alliance::Self, IsTownHall());

    //Throttle some behavior that can wait to avoid duplicate orders.
    int frames_to_skip = 4;
    if (observation->GetFoodUsed() >= observation->GetFoodCap()) {
        frames_to_skip = 6;
    }

    if (observation->GetGameLoop() % frames_to_skip != 0) {
        return;
    }

    if (!nuke_detected) {
        ManageArmy();
    }
    else {
        if (nuke_detected_frame + 400 < observation->GetGameLoop()) {
            nuke_detected = false;
        }
        Units units = observation->GetUnits(Unit::Self, IsArmy(observation));
        for (const auto& unit : units) {
            RetreatWithUnit(unit, startLocation_);
        }
    }

    BuildOrder();

    TryInjectLarva();

    ManageWorkers(UNIT_TYPEID::ZERG_DRONE, ABILITY_ID::HARVEST_GATHER, UNIT_TYPEID::ZERG_EXTRACTOR);

    ManageUpgrades();

    if (TryBuildDrone())
        return;

    if (TryBuildOverlord())
        return;

    if (observation->GetFoodArmy() < base.size() * 25) {
        BuildArmy();
    }

    if (BuildExtractor()) {
        return;
    }

    if (TryBuildExpansionHatch()) {
        return;
    }
}

void ZergMultiplayerBot::OnUnitIdle(const Unit& unit) {
    switch (unit.unit_type.ToType()) {
        case UNIT_TYPEID::ZERG_DRONE: {
            MineIdleWorkers(unit.tag, ABILITY_ID::HARVEST_GATHER,UNIT_TYPEID::ZERG_EXTRACTOR);
            break;
        }
        default:
            break;
    }
}

bool TerranMultiplayerBot::TryBuildSCV() {
    const ObservationInterface* observation = Observation();
    Units bases = observation->GetUnits(Unit::Alliance::Self, IsTownHall());

    for (const auto& base : bases) {
        if (base.unit_type == UNIT_TYPEID::TERRAN_ORBITALCOMMAND && base.energy > 50) {
            Tag mineral;
            if (FindNearestMineralPatch(base.pos, mineral)) {
                Actions()->UnitCommand(base.tag, ABILITY_ID::EFFECT_CALLDOWNMULE, mineral);
            }
        }
    }

    if (observation->GetFoodWorkers() >= max_worker_count_) {
        return false;
    }

    if (observation->GetFoodUsed() >= observation->GetFoodCap()) {
        return false;
    }

    if (observation->GetFoodWorkers() > GetExpectedWorkers(UNIT_TYPEID::TERRAN_REFINERY)) {
        return false;
    }

    for (const auto& base : bases) {
        //if there is a base with less than ideal workers
        if (base.assigned_harvesters < base.ideal_harvesters && base.build_progress == 1) {
            if (observation->GetMinerals() >= 50) {
                return TryBuildUnit(ABILITY_ID::TRAIN_SCV, base.unit_type);
            }
        }
    }
    return false;
}

bool TerranMultiplayerBot::TryBuildSupplyDepot()  {
    const ObservationInterface* observation = Observation();

    // If we are not supply capped, don't build a supply depot.
    if (observation->GetFoodUsed() < observation->GetFoodCap() - 6) {
        return false;
    }

    if (observation->GetMinerals() < 100) {
        return false;
    }

    //check to see if there is already on building
    Units units = observation->GetUnits(Unit::Alliance::Self, IsUnits(supply_depot_types));
    if (observation->GetFoodUsed() < 40) {
        for (const auto& unit : units) {
            if (unit.build_progress != 1) {
                return false;
            }
        }
    }

    // Try and build a supply depot. Find a random SCV and give it the order.
    float rx = GetRandomScalar();
    float ry = GetRandomScalar();
    Point2D build_location = Point2D(staging_location_.x + rx * 15, staging_location_.y + ry * 15);
    return TryBuildStructure(ABILITY_ID::BUILD_SUPPLYDEPOT, UNIT_TYPEID::TERRAN_SCV, build_location);
}

void TerranMultiplayerBot::BuildArmy() {
    const ObservationInterface* observation = Observation();
    //grab army and building counts
    Units barracks = observation->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::TERRAN_BARRACKS));
    Units factorys = observation->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::TERRAN_FACTORY));
    Units starports = observation->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::TERRAN_STARPORT));

    size_t widowmine_count = CountUnitTypeTotal(observation, widow_mine_types, UNIT_TYPEID::TERRAN_FACTORY, ABILITY_ID::TRAIN_WIDOWMINE);

    size_t hellbat_count = CountUnitTypeTotal(observation, hellion_types, UNIT_TYPEID::TERRAN_FACTORY, ABILITY_ID::TRAIN_HELLION);
    hellbat_count += CountUnitTypeBuilding(observation, UNIT_TYPEID::TERRAN_FACTORY, ABILITY_ID::TRAIN_HELLBAT);

    size_t siege_tank_count = CountUnitTypeTotal(observation, siege_tank_types, UNIT_TYPEID::TERRAN_FACTORY, ABILITY_ID::TRAIN_SIEGETANK);
    size_t viking_count = CountUnitTypeTotal(observation, viking_types, UNIT_TYPEID::TERRAN_FACTORY, ABILITY_ID::TRAIN_VIKINGFIGHTER);
    size_t marine_count = CountUnitTypeTotal(observation, UNIT_TYPEID::TERRAN_MARINE, UNIT_TYPEID::TERRAN_BARRACKS, ABILITY_ID::TRAIN_MARINE);
    size_t marauder_count = CountUnitTypeTotal(observation, UNIT_TYPEID::TERRAN_MARAUDER, UNIT_TYPEID::TERRAN_BARRACKS, ABILITY_ID::TRAIN_MARAUDER);
    size_t reaper_count = CountUnitTypeTotal(observation, UNIT_TYPEID::TERRAN_REAPER, UNIT_TYPEID::TERRAN_BARRACKS, ABILITY_ID::TRAIN_REAPER);
    size_t ghost_count = CountUnitTypeTotal(observation, UNIT_TYPEID::TERRAN_GHOST, UNIT_TYPEID::TERRAN_BARRACKS, ABILITY_ID::TRAIN_GHOST);
    size_t medivac_count = CountUnitTypeTotal(observation, UNIT_TYPEID::TERRAN_MEDIVAC, UNIT_TYPEID::TERRAN_STARPORT, ABILITY_ID::TRAIN_MEDIVAC);
    size_t raven_count = CountUnitTypeTotal(observation, UNIT_TYPEID::TERRAN_RAVEN, UNIT_TYPEID::TERRAN_STARPORT, ABILITY_ID::TRAIN_RAVEN);
    size_t battlecruiser_count = CountUnitTypeTotal(observation, UNIT_TYPEID::TERRAN_MEDIVAC, UNIT_TYPEID::TERRAN_STARPORT, ABILITY_ID::TRAIN_BATTLECRUISER);
    size_t banshee_count = CountUnitTypeTotal(observation, UNIT_TYPEID::TERRAN_MEDIVAC, UNIT_TYPEID::TERRAN_STARPORT, ABILITY_ID::TRAIN_BANSHEE);

    if (!mech_build_ && CountUnitType(observation, UNIT_TYPEID::TERRAN_GHOSTACADEMY) + CountUnitType(observation, UNIT_TYPEID::TERRAN_FACTORY) > 0) {
        if (!nuke_built) {
            Units ghosts = observation->GetUnits(Unit::Self, IsUnit(UNIT_TYPEID::TERRAN_GHOST));
            if (observation->GetMinerals() > 100 && observation->GetVespene() > 100) {
                TryBuildUnit(ABILITY_ID::BUILD_NUKE, UNIT_TYPEID::TERRAN_GHOSTACADEMY);
            }
            if (!ghosts.empty()) {
                AvailableAbilities abilities = Query()->GetAbilitiesForUnit(ghosts.front().tag);
                for (const auto& ability : abilities.abilities) {
                    if (ability.ability_id == ABILITY_ID::EFFECT_NUKECALLDOWN) {
                        nuke_built = true;
                    }
                }
            }

        }
    }


    if (!starports.empty()) {
        for (const auto& starport : starports) {
            if (observation->GetUnit(starport.add_on_tag) == nullptr) {
                if (mech_build_) {
                    if (starport.orders.empty() && viking_count < 5) {
                        Actions()->UnitCommand(starport.tag, ABILITY_ID::TRAIN_VIKINGFIGHTER);
                    }
                }
                else {
                    if (starport.orders.empty() && medivac_count < 5) {
                        Actions()->UnitCommand(starport.tag, ABILITY_ID::TRAIN_MEDIVAC);
                    }
                }
                continue;
            }
            else {
                if (observation->GetUnit(starport.add_on_tag)->unit_type == UNIT_TYPEID::TERRAN_STARPORTREACTOR) {
                    if (mech_build_) {
                        if (starport.orders.size() < 2 && viking_count < 5) {
                            Actions()->UnitCommand(starport.tag, ABILITY_ID::TRAIN_VIKINGFIGHTER);
                        }
                    }
                    else {
                        if (starport.orders.size() < 2 && medivac_count < 5) {
                            Actions()->UnitCommand(starport.tag, ABILITY_ID::TRAIN_MEDIVAC);
                        }
                        if (starport.orders.size() < 2 && medivac_count < 3) {
                            Actions()->UnitCommand(starport.tag, ABILITY_ID::TRAIN_MEDIVAC);
                        }
                    }

                }
                else {
                    if (starport.orders.empty() && raven_count < 2) {
                        Actions()->UnitCommand(starport.tag, ABILITY_ID::TRAIN_RAVEN);
                    }
                    if (!mech_build_) {
                        if (CountUnitType(observation, UNIT_TYPEID::TERRAN_FUSIONCORE) > 0) {
                            if (starport.orders.empty() && battlecruiser_count < 2) {
                                Actions()->UnitCommand(starport.tag, ABILITY_ID::TRAIN_BATTLECRUISER);
                                if (battlecruiser_count < 1) {
                                    return;
                                }
                            }
                        }
                        if (starport.orders.empty() && banshee_count < 2) {
                            Actions()->UnitCommand(starport.tag, ABILITY_ID::TRAIN_BANSHEE);
                        }
                    }
                }
            }
        }
    }

    if (!barracks.empty()) {
        for (const auto& barrack : barracks) {
            if (observation->GetUnit(barrack.add_on_tag) == nullptr) {
                if (!mech_build_) {
                    if (barrack.orders.empty() && marine_count < 20) {
                        Actions()->UnitCommand(barrack.tag, ABILITY_ID::TRAIN_MARINE);
                    }
                    else if (barrack.orders.empty() && observation->GetMinerals() > 1000 && observation->GetVespene() < 300) {
                        Actions()->UnitCommand(barrack.tag, ABILITY_ID::TRAIN_MARINE);
                    }
                }
            }
            else {
                if (observation->GetUnit(barrack.add_on_tag)->unit_type == UNIT_TYPEID::TERRAN_BARRACKSREACTOR) {
                    if (mech_build_) {
                        if (barrack.orders.size() < 2 && marine_count < 5) {
                            Actions()->UnitCommand(barrack.tag, ABILITY_ID::TRAIN_MARINE);
                        }
                    }
                    else{
                        if (barrack.orders.size() < 2 && marine_count < 20) {
                            Actions()->UnitCommand(barrack.tag, ABILITY_ID::TRAIN_MARINE);
                        }
                        else if (observation->GetMinerals() > 1000 && observation->GetVespene() < 300) {
                            Actions()->UnitCommand(barrack.tag, ABILITY_ID::TRAIN_MARINE);
                        }
                    }
                }
                else {
                    if (!mech_build_ && barrack.orders.empty()) {
                        if (reaper_count < 2) {
                            Actions()->UnitCommand(barrack.tag, ABILITY_ID::TRAIN_REAPER);
                        }
                        else if (ghost_count < 4) {
                            Actions()->UnitCommand(barrack.tag, ABILITY_ID::TRAIN_GHOST);
                        }
                        else if (marauder_count < 10) {
                            Actions()->UnitCommand(barrack.tag, ABILITY_ID::TRAIN_MARAUDER);
                        }
                        else {
                            Actions()->UnitCommand(barrack.tag, ABILITY_ID::TRAIN_MARINE);
                        }
                    }
                }
            }
        }
    }

    if (!factorys.empty()) {
        for (const auto& factory : factorys) {
            if (observation->GetUnit(factory.add_on_tag) == nullptr) {
                if (mech_build_) {
                    if (factory.orders.empty() && hellbat_count < 20) {
                        if (CountUnitType(observation, UNIT_TYPEID::TERRAN_ARMORY) > 0) {
                            Actions()->UnitCommand(factory.tag, ABILITY_ID::TRAIN_HELLBAT);
                        }
                        else {
                            Actions()->UnitCommand(factory.tag, ABILITY_ID::TRAIN_HELLION);
                        }
                    }
                }
            }
            else {
                if (observation->GetUnit(factory.add_on_tag)->unit_type == UNIT_TYPEID::TERRAN_FACTORYREACTOR) {
                    if (factory.orders.size() < 2 && widowmine_count < 4) {
                        Actions()->UnitCommand(factory.tag, ABILITY_ID::TRAIN_WIDOWMINE);
                    }
                    if (mech_build_ && factory.orders.size() < 2) {
                        if (observation->GetMinerals() > 1000 && observation->GetVespene() < 300) {
                            Actions()->UnitCommand(factory.tag, ABILITY_ID::TRAIN_HELLBAT);
                        }
                        if (hellbat_count < 20) {
                            if (CountUnitType(observation, UNIT_TYPEID::TERRAN_ARMORY) > 0) {
                                Actions()->UnitCommand(factory.tag, ABILITY_ID::TRAIN_HELLBAT);
                            }
                            else {
                                Actions()->UnitCommand(factory.tag, ABILITY_ID::TRAIN_HELLION);
                            }
                        }
                        if (CountUnitType(observation, UNIT_TYPEID::TERRAN_CYCLONE) < 6) {
                            Actions()->UnitCommand(factory.tag, ABILITY_ID::TRAIN_CYCLONE);
                        }
                    }
                }
                else {
                    if (mech_build_ && factory.orders.empty() && CountUnitType(observation, UNIT_TYPEID::TERRAN_ARMORY) > 0) {
                        if (CountUnitType(observation, UNIT_TYPEID::TERRAN_THOR) < 4) {
                            Actions()->UnitCommand(factory.tag, ABILITY_ID::TRAIN_THOR);
                            return;
                        }
                    }
                    if (!mech_build_ && factory.orders.empty() && siege_tank_count < 7) {
                        Actions()->UnitCommand(factory.tag, ABILITY_ID::TRAIN_SIEGETANK);
                    }
                    if (mech_build_ && factory.orders.empty() && siege_tank_count < 10) {
                        Actions()->UnitCommand(factory.tag, ABILITY_ID::TRAIN_SIEGETANK);
                    }
                }
            }
        }
    }
}

void TerranMultiplayerBot::BuildOrder() {
    const ObservationInterface* observation = Observation();
    Units bases = observation->GetUnits(Unit::Self, IsTownHall());
    Units barracks = observation->GetUnits(Unit::Self, IsUnits(barrack_types));
    Units factorys = observation->GetUnits(Unit::Self, IsUnits(factory_types));
    Units starports = observation->GetUnits(Unit::Self, IsUnits(starport_types));
    Units barracks_tech = observation->GetUnits(Unit::Self, IsUnit(UNIT_TYPEID::TERRAN_BARRACKSTECHLAB));
    Units factorys_tech = observation->GetUnits(Unit::Self, IsUnit(UNIT_TYPEID::TERRAN_FACTORYTECHLAB));
    Units starports_tech = observation->GetUnits(Unit::Self, IsUnit(UNIT_TYPEID::TERRAN_STARPORTTECHLAB));

    Units supply_depots = observation->GetUnits(Unit::Self, IsUnit(UNIT_TYPEID::TERRAN_SUPPLYDEPOT));
    if (bases.size() < 3 && CountUnitType(observation, UNIT_TYPEID::TERRAN_FUSIONCORE)) {
        TryBuildExpansionCom();
        return;
    }

    for (const auto& supply_depot : supply_depots) {
        Actions()->UnitCommand(supply_depot.tag, ABILITY_ID::MORPH_SUPPLYDEPOT_LOWER);
    }
    if (!barracks.empty()) {
        for (const auto& base : bases) {
            if (base.unit_type == UNIT_TYPEID::TERRAN_COMMANDCENTER && observation->GetMinerals() > 150) {
                Actions()->UnitCommand(base.tag, ABILITY_ID::MORPH_ORBITALCOMMAND);
            }
        }
    }

    for (const auto& barrack : barracks) {
        if (!barrack.orders.empty() || barrack.build_progress != 1) {
            continue;
        }
        if (observation->GetUnit(barrack.add_on_tag) == nullptr) {
            if (barracks_tech.size() < barracks.size() / 2 || barracks_tech.empty()) {
                TryBuildAddOn(ABILITY_ID::BUILD_TECHLAB_BARRACKS, barrack.tag);
            }
            else {
                TryBuildAddOn(ABILITY_ID::BUILD_REACTOR_BARRACKS, barrack.tag);
            }
        }
    }

    for (const auto& factory : factorys) {
        if (!factory.orders.empty()) {
            continue;
        }

        if (observation->GetUnit(factory.add_on_tag) == nullptr) {
            if (mech_build_) {
                if (factorys_tech.size() < factorys.size() / 2) {
                    TryBuildAddOn(ABILITY_ID::BUILD_TECHLAB_FACTORY, factory.tag);
                }
                else {
                    TryBuildAddOn(ABILITY_ID::BUILD_REACTOR_FACTORY, factory.tag);
                }
            }
            else {
                if (CountUnitType(observation, UNIT_TYPEID::TERRAN_BARRACKSREACTOR) < 1) {
                    TryBuildAddOn(ABILITY_ID::BUILD_REACTOR_FACTORY, factory.tag);
                }
                else {
                    TryBuildAddOn(ABILITY_ID::BUILD_TECHLAB_FACTORY, factory.tag);
                }
            }

        }
    }

    for (const auto& starport : starports) {
        if (!starport.orders.empty()) {
            continue;
        }
        if (observation->GetUnit(starport.add_on_tag) == nullptr) {
            if (mech_build_) {
                if (CountUnitType(observation,UNIT_TYPEID::TERRAN_STARPORTREACTOR) < 2) {
                    TryBuildAddOn(ABILITY_ID::BUILD_REACTOR_STARPORT, starport.tag);
                }
                else {
                    TryBuildAddOn(ABILITY_ID::BUILD_TECHLAB_STARPORT, starport.tag);
                }
            }
            else {
                if (CountUnitType(observation, UNIT_TYPEID::TERRAN_STARPORTREACTOR) < 1) {
                    TryBuildAddOn(ABILITY_ID::BUILD_REACTOR_STARPORT, starport.tag);
                }
                else {
                    TryBuildAddOn(ABILITY_ID::BUILD_TECHLAB_STARPORT, starport.tag);
                }
                
            }
        }
    }

    size_t barracks_count_target = std::min<size_t>(3 * bases.size(), 8);
    size_t factory_count_target = 1;
    size_t starport_count_target = 2;
    size_t armory_count_target = 1;
    if (mech_build_) {
        barracks_count_target = 1;
        armory_count_target = 2;
        factory_count_target = std::min<size_t>(2 * bases.size(), 7);
        starport_count_target = std::min<size_t>(1 * bases.size(), 4);
    }

    if (!factorys.empty() && starports.size() < std::min<size_t>(1 * bases.size(), 4)) {
        if (observation->GetMinerals() > 150 && observation->GetVespene() > 100) {
            TryBuildStructureRandom(ABILITY_ID::BUILD_STARPORT, UNIT_TYPEID::TERRAN_SCV);
        }
    }

    if (!barracks.empty() && factorys.size() < std::min<size_t>(2 * bases.size(), 7)) {
        if (observation->GetMinerals() > 150 && observation->GetVespene() > 100) {
            TryBuildStructureRandom(ABILITY_ID::BUILD_FACTORY, UNIT_TYPEID::TERRAN_SCV);
        }
    }

    if (barracks.size() < barracks_count_target) {
        if (observation->GetFoodWorkers() >= target_worker_count_) {
            TryBuildStructureRandom(ABILITY_ID::BUILD_BARRACKS, UNIT_TYPEID::TERRAN_SCV);
        }
    }

    if (!mech_build_) {
        if (CountUnitType(observation, UNIT_TYPEID::TERRAN_ENGINEERINGBAY) < std::min<size_t>(bases.size(), 2)) {
            if (observation->GetMinerals() > 150 && observation->GetVespene() > 100) {
                TryBuildStructureRandom(ABILITY_ID::BUILD_ENGINEERINGBAY, UNIT_TYPEID::TERRAN_SCV);
            }
        }
        if (!barracks.empty() && CountUnitType(observation, UNIT_TYPEID::TERRAN_GHOSTACADEMY) < 1) {
            if (observation->GetMinerals() > 150 && observation->GetVespene() > 50) {
                TryBuildStructureRandom(ABILITY_ID::BUILD_GHOSTACADEMY, UNIT_TYPEID::TERRAN_SCV);
            }
        }
        if (!factorys.empty() && CountUnitType(observation, UNIT_TYPEID::TERRAN_FUSIONCORE) < 1) {
            if (observation->GetMinerals() > 150 && observation->GetVespene() > 150) {
                TryBuildStructureRandom(ABILITY_ID::BUILD_FUSIONCORE, UNIT_TYPEID::TERRAN_SCV);
            }
        }
    }

    if (!barracks.empty() && CountUnitType(observation, UNIT_TYPEID::TERRAN_ARMORY) < armory_count_target) {
        if (observation->GetMinerals() > 150 && observation->GetVespene() > 100) {
            TryBuildStructureRandom(ABILITY_ID::BUILD_ARMORY, UNIT_TYPEID::TERRAN_SCV);
        }
    }
}

bool TerranMultiplayerBot::TryBuildAddOn(AbilityID ability_type_for_structure, Tag base_structure) {
    float rx = GetRandomScalar();
    float ry = GetRandomScalar();
    const Unit* unit = Observation()->GetUnit(base_structure);

    if (unit->build_progress != 1) {
        return false;
    }

    Point2D build_location = Point2D(unit->pos.x + rx * 15, unit->pos.y + ry * 15);
 
    Units units = Observation()->GetUnits(Unit::Self, IsStructure(Observation()));

    if (Query()->Placement(ability_type_for_structure, unit->pos, unit->tag)) {
        Actions()->UnitCommand(base_structure, ability_type_for_structure);
        return true;
    }

    float distance = std::numeric_limits<float>::max();
    for (const auto& u : units) {
        float d = Distance2D(u.pos, build_location);
        if (d < distance) {
            distance = d;
        }
    }
    if (distance < 6) {
        return false;
    }

    if(Query()->Placement(ability_type_for_structure,build_location,unit->tag)){
        Actions()->UnitCommand(base_structure, ability_type_for_structure, build_location);
        return true;
    }
    return false;
    
}

bool TerranMultiplayerBot::TryBuildStructureRandom(AbilityID ability_type_for_structure, UnitTypeID unit_type) {
    float rx = GetRandomScalar();
    float ry = GetRandomScalar();
    Point2D build_location = Point2D(staging_location_.x + rx * 15, staging_location_.y + ry * 15);

    Units units = Observation()->GetUnits(Unit::Self, IsStructure(Observation()));
    float distance = std::numeric_limits<float>::max();
    for (const auto& u : units) {
        if (u.unit_type == UNIT_TYPEID::TERRAN_SUPPLYDEPOTLOWERED) {
            continue;
        }
        float d = Distance2D(u.pos, build_location);
        if (d < distance) {
            distance = d;
        }
    }
    if (distance < 6) {
        return false;
    }
    return TryBuildStructure(ability_type_for_structure, unit_type, build_location);
}

void TerranMultiplayerBot::ManageUpgrades() {
    const ObservationInterface* observation = Observation();
    auto upgrades = observation->GetUpgrades();
    size_t base_count = observation->GetUnits(Unit::Alliance::Self, IsTownHall()).size();


    if (upgrades.empty()) {
        if (mech_build_) {
            TryBuildUnit(ABILITY_ID::RESEARCH_TERRANVEHICLEWEAPONS, UNIT_TYPEID::TERRAN_ARMORY);
        }
        else {
            TryBuildUnit(ABILITY_ID::RESEARCH_STIMPACK, UNIT_TYPEID::TERRAN_BARRACKSTECHLAB);
        }
    }
    else {
        for (const auto& upgrade : upgrades) {
            if (mech_build_) {
                if (upgrade == UPGRADE_ID::TERRANSHIPWEAPONSLEVEL1 && base_count > 2) {
                    TryBuildUnit(ABILITY_ID::RESEARCH_TERRANSHIPWEAPONS, UNIT_TYPEID::TERRAN_ARMORY);
                }
                else if (upgrade == UPGRADE_ID::TERRANVEHICLEWEAPONSLEVEL1 && base_count > 2) {
                    TryBuildUnit(ABILITY_ID::RESEARCH_TERRANVEHICLEWEAPONS, UNIT_TYPEID::TERRAN_ARMORY);
                }
                else if (upgrade == UPGRADE_ID::TERRANVEHICLEANDSHIPARMORSLEVEL1 && base_count > 2) {
                    TryBuildUnit(ABILITY_ID::RESEARCH_TERRANVEHICLEANDSHIPPLATING, UNIT_TYPEID::TERRAN_ARMORY);
                }
                else if (upgrade == UPGRADE_ID::TERRANVEHICLEWEAPONSLEVEL2 && base_count > 3) {
                    TryBuildUnit(ABILITY_ID::RESEARCH_TERRANVEHICLEWEAPONS, UNIT_TYPEID::TERRAN_ARMORY);
                }
                else if (upgrade == UPGRADE_ID::TERRANVEHICLEANDSHIPARMORSLEVEL2 && base_count > 3) {
                    TryBuildUnit(ABILITY_ID::RESEARCH_TERRANVEHICLEANDSHIPPLATING, UNIT_TYPEID::ZERG_SPIRE);
                }
                else {
                    TryBuildUnit(ABILITY_ID::RESEARCH_TERRANVEHICLEWEAPONS, UNIT_TYPEID::TERRAN_ARMORY);
                    TryBuildUnit(ABILITY_ID::RESEARCH_TERRANSHIPWEAPONS, UNIT_TYPEID::TERRAN_ARMORY);
                    TryBuildUnit(ABILITY_ID::RESEARCH_TERRANVEHICLEANDSHIPPLATING, UNIT_TYPEID::TERRAN_ARMORY);
                    TryBuildUnit(ABILITY_ID::RESEARCH_INFERNALPREIGNITER, UNIT_TYPEID::TERRAN_FACTORYTECHLAB);
                }
            }//Not mech build only
            else {
                if (CountUnitType(observation, UNIT_TYPEID::TERRAN_ARMORY) > 0) {
                    if (upgrade == UPGRADE_ID::TERRANINFANTRYWEAPONSLEVEL1 && base_count > 2) {
                        TryBuildUnit(ABILITY_ID::RESEARCH_TERRANINFANTRYWEAPONS, UNIT_TYPEID::TERRAN_ENGINEERINGBAY);
                    }
                    else if (upgrade == UPGRADE_ID::TERRANINFANTRYARMORSLEVEL1 && base_count > 2) {
                        TryBuildUnit(ABILITY_ID::RESEARCH_TERRANINFANTRYARMOR, UNIT_TYPEID::TERRAN_ENGINEERINGBAY);
                    }
                    if (upgrade == UPGRADE_ID::TERRANINFANTRYWEAPONSLEVEL2 && base_count > 4) {
                        TryBuildUnit(ABILITY_ID::RESEARCH_TERRANINFANTRYWEAPONS, UNIT_TYPEID::TERRAN_ENGINEERINGBAY);
                    }
                    else if (upgrade == UPGRADE_ID::TERRANINFANTRYARMORSLEVEL2 && base_count > 4) {
                        TryBuildUnit(ABILITY_ID::RESEARCH_TERRANINFANTRYARMOR, UNIT_TYPEID::TERRAN_ENGINEERINGBAY);
                    }
                }
                TryBuildUnit(ABILITY_ID::RESEARCH_STIMPACK, UNIT_TYPEID::TERRAN_BARRACKSTECHLAB);
                TryBuildUnit(ABILITY_ID::RESEARCH_TERRANINFANTRYWEAPONS, UNIT_TYPEID::TERRAN_ENGINEERINGBAY);
                TryBuildUnit(ABILITY_ID::RESEARCH_TERRANINFANTRYARMOR, UNIT_TYPEID::TERRAN_ENGINEERINGBAY);
                TryBuildUnit(ABILITY_ID::RESEARCH_STIMPACK, UNIT_TYPEID::TERRAN_BARRACKSTECHLAB);
                TryBuildUnit(ABILITY_ID::RESEARCH_COMBATSHIELD, UNIT_TYPEID::TERRAN_BARRACKSTECHLAB);
                TryBuildUnit(ABILITY_ID::RESEARCH_CONCUSSIVESHELLS, UNIT_TYPEID::TERRAN_BARRACKSTECHLAB);
                TryBuildUnit(ABILITY_ID::RESEARCH_PERSONALCLOAKING, UNIT_TYPEID::TERRAN_GHOSTACADEMY);
                TryBuildUnit(ABILITY_ID::RESEARCH_BANSHEECLOAKINGFIELD, UNIT_TYPEID::TERRAN_STARPORTTECHLAB);
            }
        }
    }
}

void TerranMultiplayerBot::ManageArmy() {

    const ObservationInterface* observation = Observation();

    Units enemy_units = observation->GetUnits(Unit::Alliance::Enemy);

    Units army = observation->GetUnits(Unit::Alliance::Self, IsArmy(observation));
    int wait_til_supply = 100;
    if (mech_build_) {
        wait_til_supply = 110;
    }

    Units nuke = observation->GetUnits(Unit::Self, IsUnit(UNIT_TYPEID::TERRAN_NUKE));
    for (const auto& unit : army) {
         if (enemy_units.empty() && observation->GetFoodArmy() < wait_til_supply) {
            switch (unit.unit_type.ToType()) {
                case UNIT_TYPEID::TERRAN_SIEGETANKSIEGED: {
                    Actions()->UnitCommand(unit.tag, ABILITY_ID::MORPH_UNSIEGE);
                    break;
                }
                default:
                    RetreatWithUnit(unit, staging_location_);
                    break;
            }
        }
        else if (!enemy_units.empty()) {
            switch (unit.unit_type.ToType()) {
                case UNIT_TYPEID::TERRAN_WIDOWMINE: {
                    float distance = std::numeric_limits<float>::max();
                    for (const auto& u : enemy_units) {
                        float d = Distance2D(u.pos, unit.pos);
                        if (d < distance) {
                            distance = d;
                        }
                    }
                    if (distance < 6) {
                        Actions()->UnitCommand(unit.tag, ABILITY_ID::BURROWDOWN);
                    }
                    break;
                }
                case UNIT_TYPEID::TERRAN_MARINE: {
                    if (stim_researched_ && !unit.orders.empty()) {
                        if (unit.orders.front().ability_id == ABILITY_ID::ATTACK) {
                            float distance = std::numeric_limits<float>::max();
                            for (const auto& u : enemy_units) {
                                float d = Distance2D(u.pos, unit.pos);
                                if (d < distance) {
                                    distance = d;
                                }
                            }
                            bool has_stimmed = false;
                            for (const auto& buff : unit.buffs) {
                                if (buff == BUFF_ID::STIMPACK) {
                                    has_stimmed = true;
                                }
                            }
                            if (distance < 6 && !has_stimmed) {
                                Actions()->UnitCommand(unit.tag, ABILITY_ID::EFFECT_STIM);
                                break;
                            }
                        }

                    }
                    AttackWithUnit(unit, observation);
                    break;
                }
                case UNIT_TYPEID::TERRAN_MARAUDER: {
                    if (stim_researched_ && !unit.orders.empty()) {
                        if (unit.orders.front().ability_id == ABILITY_ID::ATTACK) {
                            float distance = std::numeric_limits<float>::max();
                            for (const auto& u : enemy_units) {
                                float d = Distance2D(u.pos, unit.pos);
                                if (d < distance) {
                                    distance = d;
                                }
                            }
                            bool has_stimmed = false;
                            for (const auto& buff : unit.buffs) {
                                if (buff == BUFF_ID::STIMPACK) {
                                    has_stimmed = true;
                                }
                            }
                            if (distance < 7 && !has_stimmed) {
                                Actions()->UnitCommand(unit.tag, ABILITY_ID::EFFECT_STIM);
                                break;
                            }
                        }
                    }
                    AttackWithUnit(unit, observation);
                    break;
                }
                case UNIT_TYPEID::TERRAN_GHOST: {
                    float distance = std::numeric_limits<float>::max();
                    Unit closest_unit;
                    for (const auto& u : enemy_units) {
                        float d = Distance2D(u.pos, unit.pos);
                        if (d < distance) {
                            distance = d;
                            closest_unit = u;
                        }
                    }
                    if (ghost_cloak_researched_) {
                        if (distance < 7 && unit.energy > 50) {
                            Actions()->UnitCommand(unit.tag, ABILITY_ID::BEHAVIOR_CLOAKON);
                            break;
                        }
                    }
                    if (nuke_built) {
                        Actions()->UnitCommand(unit.tag, ABILITY_ID::EFFECT_NUKECALLDOWN, closest_unit.pos);
                    }
                    else if (unit.energy > 50 && !unit.orders.empty()) {
                        if(unit.orders.front().ability_id == ABILITY_ID::ATTACK)
                        Actions()->UnitCommand(unit.tag, ABILITY_ID::EFFECT_GHOSTSNIPE, unit.engaged_target_tag);
                        break;
                    }
                    AttackWithUnit(unit, observation);
                    break;
                }
                case UNIT_TYPEID::TERRAN_SIEGETANK: {
                    float distance = std::numeric_limits<float>::max();
                    for (const auto& u : enemy_units) {
                        float d = Distance2D(u.pos, unit.pos);
                        if (d < distance) {
                            distance = d;
                        }
                    }
                    if (distance < 11) {
                        Actions()->UnitCommand(unit.tag, ABILITY_ID::MORPH_SIEGEMODE);
                    }
                    else {
                        AttackWithUnit(unit, observation);
                    }
                    break;
                }
                case UNIT_TYPEID::TERRAN_SIEGETANKSIEGED: {
                    float distance = std::numeric_limits<float>::max();
                    for (const auto& u : enemy_units) {
                        float d = Distance2D(u.pos, unit.pos);
                        if (d < distance) {
                            distance = d;
                        }
                    }
                    if (distance > 13) {
                        Actions()->UnitCommand(unit.tag, ABILITY_ID::MORPH_UNSIEGE);
                    }
                    else {
                        AttackWithUnit(unit, observation);
                    }
                    break;
                }
                case UNIT_TYPEID::TERRAN_MEDIVAC: {
                    Units bio_units = observation->GetUnits(Unit::Self, IsUnits(bio_types));
                    if (unit.orders.empty()) {
                        for (const auto& bio_unit : bio_units) {
                            if (bio_unit.health < bio_unit.health_max) {
                                Actions()->UnitCommand(unit.tag, ABILITY_ID::EFFECT_HEAL, bio_unit.tag);
                                break;
                            }
                        }
                        if (!bio_units.empty()) {
                            Actions()->UnitCommand(unit.tag, ABILITY_ID::ATTACK, bio_units.front().tag);
                        }
                    }
                    break;
                }
                case UNIT_TYPEID::TERRAN_VIKINGFIGHTER: {
                    Units flying_units = observation->GetUnits(Unit::Enemy, IsFlying());
                    if (flying_units.empty()) {
                        Actions()->UnitCommand(unit.tag, ABILITY_ID::MORPH_VIKINGASSAULTMODE);
                    }
                    else {
                        Actions()->UnitCommand(unit.tag, ABILITY_ID::ATTACK, flying_units.front());
                    }
                    break;
                }
                case UNIT_TYPEID::TERRAN_VIKINGASSAULT: {
                    Units flying_units = observation->GetUnits(Unit::Enemy, IsFlying());
                    if (!flying_units.empty()) {
                        Actions()->UnitCommand(unit.tag, ABILITY_ID::MORPH_VIKINGFIGHTERMODE);
                    }
                    else {
                        AttackWithUnit(unit, observation);
                    }
                    break;
                }
                case UNIT_TYPEID::TERRAN_CYCLONE: {
                    Units flying_units = observation->GetUnits(Unit::Enemy, IsFlying());
                    if (!flying_units.empty() && unit.orders.empty()) {
                        Actions()->UnitCommand(unit.tag, ABILITY_ID::EFFECT_LOCKON, flying_units.front());
                    }
                    else if (!flying_units.empty() && !unit.orders.empty()) {
                        if (unit.orders.front().ability_id != ABILITY_ID::EFFECT_LOCKON) {
                            Actions()->UnitCommand(unit.tag, ABILITY_ID::EFFECT_LOCKON, flying_units.front());
                        }
                    }
                    else {
                        AttackWithUnit(unit, observation);
                    }
                    break;
                }
                case UNIT_TYPEID::TERRAN_HELLION: {
                    if (CountUnitType(observation, UNIT_TYPEID::TERRAN_ARMORY) > 0) {
                        Actions()->UnitCommand(unit.tag, ABILITY_ID::MORPH_HELLBAT);
                    }
                    AttackWithUnit(unit, observation);
                    break;
                }
                case UNIT_TYPEID::TERRAN_BANSHEE: {
                    if (banshee_cloak_researched_) {
                        float distance = std::numeric_limits<float>::max();
                        for (const auto& u : enemy_units) {
                            float d = Distance2D(u.pos, unit.pos);
                            if (d < distance) {
                                distance = d;
                            }
                        }
                        if (distance < 7 && unit.energy > 50) {
                            Actions()->UnitCommand(unit.tag, ABILITY_ID::BEHAVIOR_CLOAKON);
                        }
                    }
                    AttackWithUnit(unit, observation);
                    break;
                }
                case UNIT_TYPEID::TERRAN_RAVEN: {
                    if (unit.energy > 125) {
                        Actions()->UnitCommand(unit.tag, ABILITY_ID::EFFECT_HUNTERSEEKERMISSILE, enemy_units.front());
                        break;
                    }
                    if (unit.orders.empty()) {
                        Actions()->UnitCommand(unit.tag, ABILITY_ID::ATTACK, army.front().pos);
                    }
                    break;
                }
                default: {
                    AttackWithUnit(unit, observation);
                }
                }
        }
        else {
            switch (unit.unit_type.ToType()) {
                case UNIT_TYPEID::TERRAN_SIEGETANKSIEGED: {
                    Actions()->UnitCommand(unit.tag, ABILITY_ID::MORPH_UNSIEGE);
                    break;
                }
                case UNIT_TYPEID::TERRAN_MEDIVAC: {
                    Units bio_units = observation->GetUnits(Unit::Self, IsUnits(bio_types));
                    if (unit.orders.empty()) {
                        Actions()->UnitCommand(unit.tag, ABILITY_ID::ATTACK, bio_units.front().pos);
                    }
                    break;
                }
                default:
                    ScoutWithUnit(unit, observation);
                    break;
            }
        }
    }
}

bool TerranMultiplayerBot::TryBuildExpansionCom() {
    const ObservationInterface* observation = Observation();
    Units bases = observation->GetUnits(Unit::Alliance::Self, IsTownHall());
    //Don't have more active bases than we can provide workers for
    if (GetExpectedWorkers(UNIT_TYPEID::TERRAN_REFINERY) > max_worker_count_) {
        return false;
    }
    // If we have extra workers around, try and build another Hatch.
    if (GetExpectedWorkers(UNIT_TYPEID::TERRAN_REFINERY) < observation->GetFoodWorkers() - 10) {
        return TryExpand(ABILITY_ID::BUILD_COMMANDCENTER, UNIT_TYPEID::TERRAN_SCV);
    }
    //Only build another Hatch if we are floating extra minerals
    if (observation->GetMinerals() > std::min<size_t>(bases.size() * 400, 1200)) {
        return TryExpand(ABILITY_ID::BUILD_COMMANDCENTER, UNIT_TYPEID::TERRAN_SCV);
    }
    return false;
}

bool TerranMultiplayerBot::BuildRefinery() {
    const ObservationInterface* observation = Observation();
    Units bases = observation->GetUnits(Unit::Alliance::Self, IsTownHall());

    if (CountUnitType(observation, UNIT_TYPEID::TERRAN_REFINERY) >= observation->GetUnits(Unit::Alliance::Self, IsTownHall()).size() * 2) {
        return false;
    }

    for (const auto& base : bases) {
        if (base.assigned_harvesters >= base.ideal_harvesters) {
            if (base.build_progress == 1) {
                if (TryBuildGas(ABILITY_ID::BUILD_REFINERY, UNIT_TYPEID::TERRAN_SCV, base.pos)) {
                    return true;
                }
            }
        }
    }
    return false;
}
void TerranMultiplayerBot::OnStep() {


    const ObservationInterface* observation = Observation();
    Units units = observation->GetUnits(Unit::Self, IsArmy(observation));
    Units nukes = observation->GetUnits(Unit::Self, IsUnit(UNIT_TYPEID::TERRAN_NUKE));

    //Throttle some behavior that can wait to avoid duplicate orders.
    int frames_to_skip = 4;
    if (observation->GetFoodUsed() >= observation->GetFoodCap()) {
        frames_to_skip = 6;
    }

    if (observation->GetGameLoop() % frames_to_skip != 0) {
        return;
    }

    if (!nuke_detected && nukes.empty()) {
        ManageArmy();
    }
    else {
        if (nuke_detected_frame + 400 < observation->GetGameLoop()) {
            nuke_detected = false;
        }
        for (const auto& unit : units) {
            RetreatWithUnit(unit, startLocation_);
        }
    }

    BuildOrder();

    ManageWorkers(UNIT_TYPEID::TERRAN_SCV, ABILITY_ID::HARVEST_GATHER, UNIT_TYPEID::TERRAN_REFINERY);

    ManageUpgrades();

    if (TryBuildSCV())
        return;

    if (TryBuildSupplyDepot())
        return;

    BuildArmy();

    if (BuildRefinery()) {
        return;
    }

    if (TryBuildExpansionCom()) {
        return;
    }
}

void TerranMultiplayerBot::OnUnitIdle(const Unit& unit) {
    switch (unit.unit_type.ToType()) {
        case UNIT_TYPEID::TERRAN_SCV: {
            MineIdleWorkers(unit.tag, ABILITY_ID::HARVEST_GATHER, UNIT_TYPEID::TERRAN_REFINERY);
            break;
        }
        default:
            break;
    }
}

void TerranMultiplayerBot::OnUpgradeCompleted(UpgradeID upgrade) {
    switch (upgrade.ToType()) {
        case UPGRADE_ID::STIMPACK: {
            stim_researched_ = true;
        }
        case UPGRADE_ID::PERSONALCLOAKING: {
            ghost_cloak_researched_ = true;
        }
        case UPGRADE_ID::BANSHEECLOAK: {
            banshee_cloak_researched_ = true;
        }
        default:
            break;
    }
}

void TerranBot::PrintStatus(std::string msg) {
    int64_t bot_identifier = int64_t(this) & 0xFFFLL;
    std::cout << std::to_string(bot_identifier) << ": " << msg << std::endl;
}

void TerranBot::OnGameStart() {
    game_info_ = Observation()->GetGameInfo();
    PrintStatus("game started.");
}

// Tries to find a random location that can be pathed to on the map.
// Returns 'true' if a new, random location has been found that is pathable by the unit.
bool TerranBot::FindEnemyPosition(Tag, Point2D& target_pos) {
    if (game_info_.enemy_start_locations.empty()) return false;
    target_pos = game_info_.enemy_start_locations.front();
    return true;
}

void TerranBot::ScoutWithMarines() {

    Units units = Observation()->GetUnits(Unit::Alliance::Self);
    for (const auto& unit : units) {
        UnitTypeID unit_type(unit.unit_type);
        if (unit_type != UNIT_TYPEID::TERRAN_MARINE)
            continue;

        if (!unit.orders.empty())
            continue;

        // Priority to attacking enemy structures.
        Unit enemy_unit;
        if (FindEnemyStructure(Observation(), enemy_unit)) {
            assert(enemy_unit.tag);
            Actions()->UnitCommand(unit, ABILITY_ID::ATTACK, enemy_unit.tag);
            return;
        }

        Point2D target_pos;
        // TODO: For efficiency, these queries should be batched.
        if (FindEnemyPosition(unit.tag, target_pos)) {
            Actions()->UnitCommand(unit, ABILITY_ID::SMART, target_pos);
        }
    }
}

bool TerranBot::TryBuildStructure(AbilityID ability_type_for_structure, UnitTypeID unit_type) {
    const ObservationInterface* observation = Observation();

    // If a unit already is building a supply structure of this type, do nothing.
    Units units = observation->GetUnits(Unit::Alliance::Self);
    for (const auto& unit : units) {
        for (const auto& order : unit.orders) {
            if (order.ability_id == ability_type_for_structure) {
                return false;
            }
        }
    }

    // Just try a random location near the unit.
    Unit unit;
    if (!GetRandomUnit(unit, observation, unit_type))
        return false;

    float rx = GetRandomScalar();
    float ry = GetRandomScalar();

    Actions()->UnitCommand(unit, ability_type_for_structure, Point2D(unit.pos.x + rx * 15.0f, unit.pos.y + ry * 15.0f));
    return true;
}

bool TerranBot::TryBuildSupplyDepot() {
    const ObservationInterface* observation = Observation();

    // If we are not supply capped, don't build a supply depot.
    if (observation->GetFoodUsed() <= observation->GetFoodCap() - 2)
        return false;

    // Try and build a depot. Find a random TERRAN_SCV and give it the order.
    return TryBuildStructure(ABILITY_ID::BUILD_SUPPLYDEPOT);
}

bool TerranBot::TryBuildBarracks() {
    const ObservationInterface* observation = Observation();

    // Wait until we have our quota of TERRAN_SCV's.
    if (CountUnitType(observation, UNIT_TYPEID::TERRAN_SCV) < TargetSCVCount)
        return false;

    // One build 1 barracks.
    if (CountUnitType(observation, UNIT_TYPEID::TERRAN_BARRACKS) > 0)
        return false;

    return TryBuildStructure(ABILITY_ID::BUILD_BARRACKS);
}

bool TerranBot::TryBuildUnit(AbilityID ability_type_for_unit, UnitTypeID unit_type) {
    const ObservationInterface* observation = Observation();

    Unit unit;
    if (!GetRandomUnit(unit, observation, unit_type))
        return false;

    if (!unit.orders.empty())
        return false;

    Actions()->UnitCommand(unit.tag, ability_type_for_unit);
    return true;
}

bool TerranBot::TryBuildSCV() {
    if (CountUnitType(Observation(), UNIT_TYPEID::TERRAN_SCV) >= TargetSCVCount)
        return false;

    return TryBuildUnit(ABILITY_ID::TRAIN_SCV, UNIT_TYPEID::TERRAN_COMMANDCENTER);
}

bool TerranBot::TryBuildMarine() {
    return TryBuildUnit(ABILITY_ID::TRAIN_MARINE, UNIT_TYPEID::TERRAN_BARRACKS);
}

void TerranBot::OnStep() {
    // If there are marines and the command center is not found, send them scouting.
    ScoutWithMarines();

    // Build supply depots if they are needed.
    if (TryBuildSupplyDepot())
        return;

    // Build TERRAN_SCV's if they are needed.
    if (TryBuildSCV())
        return;

    // Build Barracks if they are ready to be built.
    if (TryBuildBarracks())
        return;

    // Just keep building marines if possible.
    if (TryBuildMarine())
        return;
}

void TerranBot::OnGameEnd() {
    std::cout << "Game Ended for: " << std::to_string(Control()->Proto().GetAssignedPort()) << std::endl;
}

void MarineMicroBot::OnGameStart() {
    move_back_ = false;
    targeted_zergling_ = 0;
}

void MarineMicroBot::OnStep() {
    const ObservationInterface* observation = Observation();
    ActionInterface* action = Actions();

    Point2D mp, zp;

    if (!GetPosition(UNIT_TYPEID::TERRAN_MARINE, Unit::Alliance::Self, mp)) {
        return;
    }

    if (!GetPosition(UNIT_TYPEID::ZERG_ZERGLING, Unit::Alliance::Enemy, zp)) {
        return;
    }

    if (!GetNearestZergling(mp)) {
        return;
    }

    Units units = observation->GetUnits(Unit::Alliance::Self);
    for (const auto& u : units) {
        switch (static_cast<UNIT_TYPEID>(u.unit_type)) {
            case UNIT_TYPEID::TERRAN_MARINE: {
                if (!move_back_) {
                    action->UnitCommand(u, ABILITY_ID::ATTACK, targeted_zergling_);
                }
                else {
                    if (Distance2D(mp, backup_target_) < 1.5f) {
                        move_back_ = false;
                    }

                    action->UnitCommand(u, ABILITY_ID::SMART, backup_target_);
                }
                break;
            }
            default: {
                break;
            }
        }
    }
}

void MarineMicroBot::OnUnitDestroyed(const Unit& unit) {
    if (unit.tag == targeted_zergling_) {
        Point2D mp, zp;
        if (!GetPosition(UNIT_TYPEID::TERRAN_MARINE, Unit::Alliance::Self, mp)) {
            return;
        }

        if (!GetPosition(UNIT_TYPEID::ZERG_ZERGLING, Unit::Alliance::Enemy, zp)) {
            return;
        }

        Vector2D diff = mp - zp;
        Normalize2D(diff);

        targeted_zergling_ = 0;
        move_back_ = true;
        backup_start_ = mp;
        backup_target_ = mp + diff * 3.0f;
    }
}

bool MarineMicroBot::GetPosition(UNIT_TYPEID unit_type, Unit::Alliance alliace, Point2D& position) {
    const ObservationInterface* observation = Observation();
    Units units = observation->GetUnits(alliace);

    if (units.empty()) {
        return false;
    }

    position = Point2D(0.0f, 0.0f);
    unsigned int count = 0;

    for (const auto& u : units) {
        if (u.unit_type == unit_type) {
            position.x += u.pos.x;
            position.y += u.pos.y;
            ++count;
        }
    }

    position.x /= count;
    position.y /= count;

    return true;
}

bool MarineMicroBot::GetNearestZergling(const Point2D& from) {
    const ObservationInterface* observation = Observation();
    Units units = observation->GetUnits(Unit::Alliance::Enemy);

    if (units.empty()) {
        return false;
    }

    float distance = std::numeric_limits<float>::max();
    for (const auto& u : units) {
        if (u.unit_type == UNIT_TYPEID::ZERG_ZERGLING) {
            float d = DistanceSquared2D(u.pos, from);
            if (d < distance) {
                distance = d;
                targeted_zergling_ = u.tag;
            }
        }
    }

    return true;
}
}
