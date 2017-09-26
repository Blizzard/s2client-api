Tutorial 2: SCVs and Supply Depots
==================================

The goal in this tutorial will be to build scvs and supply depots.

1. Building SCVs.
2. Building Supply Depots.
3. Managing idle SCVs.

As a starting point you should have the code from the end of tutorial 1 compiling and running.
This tutorial will start where the last one left off.

Building SCVs
-------------

The library provides many events for your convenience, a big one we will use for this tutorial is 
the OnUnitIdle event. This function will get called each time a unit has been built and has no orders
or the unit had orders in the previous step and currently does not.

> In both the Starcraft 2 engine and library both buildings and units are considered units and 
> are represented with a Unit object. 

Add the following code to your Bot class.

```C++
// In your bot class.
virtual void OnUnitIdle(const Unit* unit) final {
    switch (unit->unit_type.ToType()) {
        case UNIT_TYPEID::TERRAN_COMMANDCENTER: {
            Actions()->UnitCommand(unit, ABILITY_ID::TRAIN_SCV);
            break;
        }
        default: {
            break;
        }
    }
}
```

As you can see we are going to try to build an SCV with the idle unit if it is a Command Center.

> The UNIT_TYPEID enum holds the ids of common units you are likely to find in 1v1 matches.
> Feel free to look at sc2_typeenums.h to see a list of units and their corresponding id.

Building Supply Depots
----------------------

Compile and run your bot now. You will see the Command Center making scvs up until the supply cap.
That seems pretty good! We just need to build some supply depots now, lets replace the code in OnStep
with the following.

```C++
virtual void OnStep() {
    TryBuildSupplyDepot();
}
```

Implement TryBuildSupplyDepot and TryBuildStructure as functions of our bot class.

```C++
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
```

Compile and run your bot now. It's mining and building supply depots, it's almost ready for ladder!
You'll notice when the SCV is done building a supply depot it sits idle, how useless. Lets fix that now.

Managing Idle SCVs
------------------

We have already hooked into the on idle event for building SCVs, we can use that same function to manage idle ones.
Refactor your OnUnitIdle function with the following.

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
        default: {
            break;
        }
    }
}
```

> The ability type of SMART is equivalent to a right click in Starcraft 2 when you have a unit selected.

Now we just need to implement FindNearestMineralPatch and we can fix our lazy SCV.

```C++
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

```

Exercises
---------

> These exercises are very optional, so feel free to move onto the next tutorial.
> Otherwise, they act as a fun way to discover more about the API.

1. As you build more scvs you'll want to start building supply depots at a higher rate. Try modifying the code to build multiple
supply depots instead of just 1 at a time.
2. (Challenging) Build two refineries and start mining gas. You can use code similar to FindNearestMineralPatch to
find a geyser. You'll then want to detect when the refinery is either created or becomes idle and begin gathering gas with 3 scvs.


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
            default: {
                break;
            }
        }
    }
private:
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
