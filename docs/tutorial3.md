Tutorial 3: Building Marines and Scouting
=========================================

So far we've been focused on some of the simpler macro abilities required to make a bot. You've gotten your
bot to build scv's and supply depots when it's running near the supply cap. In this tutorial we'll learn
the following.

1. Building a Barracks.
2. Building Marines from said Barracks.
3. Scouting Your Opponent.

This tutorial builds on the previous so make sure the code from the last tutorial is copy pasted into
tutorial.cc and you can build/run it.

Building a Barracks
-------------------

The first step to any good Terran build is to build marines, and to build marines we must first build a barracks.
We've already seen how to construct supply depots so lets create some similar code for constructing a barracks.
Modify your OnStep function to the following.

```C++
virtual void OnStep() final {
    TryBuildSupplyDepot();

    TryBuildBarracks();
}
```

> We'll implement TryBuildBarracks shortly.

First, we have some constraints we must satisfy to build a barracks, primarily, we need a supply depot. We'd also like to
only build one for this this tutorial so lets create a helper function for counting unit types and we'll use that in
TryBuildBarracks in order to determine if we should build one or not.

```C++
size_t CountUnitType(UNIT_TYPEID unit_type) {
    return Observation()->GetUnits(Unit::Alliance::Self, IsUnit(unit_type)).size();
}
```

That function is counting the number of a certain unit type the player owns. GetUnits takes a Filter parameter that 
allows you to remove units that don't meet a certain condition. In this case that condition is that the units are of 
the desired unit_type.


We now have the necessary helper functions to implement TryBuildBarracks.

```C++
bool TryBuildBarracks() {
    const ObservationInterface* observation = Observation();

    if (CountUnitType(UNIT_TYPEID::TERRAN_SUPPLYDEPOT) < 1) {
        return false;
    }

    if (CountUnitType(UNIT_TYPEID::TERRAN_BARRACKS) > 0) {
        return false;
    }

    return TryBuildStructure(ABILITY_ID::BUILD_BARRACKS);
}
```

You can build and run your code at this point, if you'd like, you should see your bot building a barracks after it completes
its first supply depot. We'd now like that barracks to actually do something. Recall we've overwritten a OnUnitIdle event in
an earlier tutorial, completion of the barracks should trigger that event!

Building Marines
----------------

Similar to how we construct SCVs we can now produce marines. Add the following code to the switch case in OnUnitIdle.
The entire function should look like the following, the new code is the UNIT_TYPE::TERRAN_BARRACKS case:

```C++
virtual void OnUnitIdle(const Unit* unit) final {
    switch (unit->unit_type.ToType()) {
        case UNIT_TYPEID::TERRAN_COMMANDCENTER: {
            Actions()->UnitCommand(unit, ABILITY_ID::TRAIN_SCV);
            break;
        }
        case UNIT_TYPEID::TERRAN_SCV: {
            const Unit* mineral_target = FindNearestMineralPatch(unit->pos);
            if (!mineral_target) {
                break;
            }
            Actions()->UnitCommand(unit, ABILITY_ID::SMART, mineral_target);
            break;
        }
        case UNIT_TYPEID::TERRAN_BARRACKS: {
            Actions()->UnitCommand(unit, ABILITY_ID::TRAIN_MARINE);
            break;
        }
        case UNIT_TYPEID::TERRAN_MARINE: {
            const GameInfo& game_info = Observation()->GetGameInfo();
            Actions()->UnitCommand(unit, ABILITY_ID::ATTACK_ATTACK, game_info.enemy_start_locations.front());
            break;
        }
        default: {
            break;
        }
    }
}

```

Notice how easy that is! In general, OnUnitIdle is an excellent function to add code to to control unit production and orders.
At this point if you build and run the code your bot should build a barracks and start producing marines with them. Our last step
should be to scout the enemy.

Scouting Your Opponent
----------------------

In a normal match when the game begins the minimap is pinged with all possible starting locations of enemies, the api contains
that same information in the ObservationInterface. You can retrieve it via GetGameInfo(). Lets use that function in our OnUnitIdle
so a newly spawned marine will attack move towards the enemy as soon as it's spawned. It will be fun to see countless marines
walk to their demise.

In your OnUnitIdle add the following code to your switch case -

```C++
case UNIT_TYPEID::TERRAN_MARINE: {
    const GameInfo& game_info = Observation()->GetGameInfo();
    Actions()->UnitCommand(unit, ABILITY_ID::ATTACK_ATTACK, game_info.enemy_start_locations.front());
    break;
}
```

How fun, build and run and you can watch marines endlessly walk to their death.

Exercises
----------

1. Try building and producing marines from three barracks instead of one.
2. (Challenging) Perform a simple rush, from your three barracks wait until you've gathered
10-20 marines then attack move to your enemy.

Full Source Code
----------------

```C++
#include <sc2api/sc2_api.h>

#include <iostream>

using namespace sc2;

class Bot : public Agent {
public:
    virtual void OnGameStart() final {
        std::cout << "Hello, World!" << std::endl;
    }

    virtual void OnStep() final {
        TryBuildSupplyDepot();

        TryBuildBarracks();
    }

    virtual void OnUnitIdle(const Unit* unit) final {
        switch (unit->unit_type.ToType()) {
            case UNIT_TYPEID::TERRAN_COMMANDCENTER: {
                Actions()->UnitCommand(unit, ABILITY_ID::TRAIN_SCV);
                break;
            }
            case UNIT_TYPEID::TERRAN_SCV: {
                const Unit* mineral_target = FindNearestMineralPatch(unit->pos);
                if (!mineral_target) {
                    break;
                }
                Actions()->UnitCommand(unit, ABILITY_ID::SMART, mineral_target);
                break;
            }
            case UNIT_TYPEID::TERRAN_BARRACKS: {
                Actions()->UnitCommand(unit, ABILITY_ID::TRAIN_MARINE);
                break;
            }
            case UNIT_TYPEID::TERRAN_MARINE: {
                const GameInfo& game_info = Observation()->GetGameInfo();
                Actions()->UnitCommand(unit, ABILITY_ID::ATTACK_ATTACK, game_info.enemy_start_locations.front());
                break;
            }
            default: {
                break;
            }
        }
    }
private:
    size_t CountUnitType(UNIT_TYPEID unit_type) {
        return Observation()->GetUnits(Unit::Alliance::Self, IsUnit(unit_type)).size();
    }

    bool TryBuildStructure(ABILITY_ID ability_type_for_structure, UNIT_TYPEID unit_type = UNIT_TYPEID::TERRAN_SCV) {
        const ObservationInterface* observation = Observation();

        // If a unit already is building a supply structure of this type, do nothing.
        // Also get an scv to build the structure.
        const Unit* unit_to_build = nullptr;
        Units units = observation->GetUnits(Unit::Alliance::Self);
        for (const auto& unit : units) {
            for (const auto& order : unit->orders) {
                if (order.ability_id == ability_type_for_structure) {
                    return false;
                }
            }

            if (unit->unit_type == unit_type) {
                unit_to_build = unit;
            }
        }

        float rx = GetRandomScalar();
        float ry = GetRandomScalar();

        Actions()->UnitCommand(unit_to_build,
            ability_type_for_structure,
            Point2D(unit_to_build->pos.x + rx * 15.0f, unit_to_build->pos.y + ry * 15.0f));

        return true;
    }

    bool TryBuildSupplyDepot() {
        const ObservationInterface* observation = Observation();

        // If we are not supply capped, don't build a supply depot.
        if (observation->GetFoodUsed() <= observation->GetFoodCap() - 2)
            return false;

        // Try and build a depot. Find a random SCV and give it the order.
        return TryBuildStructure(ABILITY_ID::BUILD_SUPPLYDEPOT);
    }

    const Unit* FindNearestMineralPatch(const Point2D& start) {
        Units units = Observation()->GetUnits(Unit::Alliance::Neutral);
        float distance = std::numeric_limits<float>::max();
        const Unit* target = nullptr;
        for (const auto& u : units) {
            if (u->unit_type == UNIT_TYPEID::NEUTRAL_MINERALFIELD) {
                float d = DistanceSquared2D(u->pos, start);
                if (d < distance) {
                    distance = d;
                    target = u;
                }
            }
        }
        return target;
    }

    bool TryBuildBarracks() {
        const ObservationInterface* observation = Observation();

        if (CountUnitType(UNIT_TYPEID::TERRAN_SUPPLYDEPOT) < 1) {
            return false;
        }

        if (CountUnitType(UNIT_TYPEID::TERRAN_BARRACKS) > 0) {
            return false;
        }

        return TryBuildStructure(ABILITY_ID::BUILD_BARRACKS);
    }
};

int main(int argc, char* argv[]) {
    Coordinator coordinator;
    coordinator.LoadSettings(argc, argv);

    Bot bot;
    coordinator.SetParticipants({
        CreateParticipant(Race::Terran, &bot),
        CreateComputer(Race::Zerg)
    });

    coordinator.LaunchStarcraft();
    coordinator.StartGame(sc2::kMapBelShirVestigeLE);

    while (coordinator.Update()) {
    }

    return 0;
}
```
