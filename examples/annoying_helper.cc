#include "sc2api/sc2_api.h"
#include "sc2lib/sc2_lib.h"

#include "sc2utils/sc2_manage_process.h"

// A helper bot that actually just moves a random unit every 5 seconds
// timing is controlled in Main, with the SleepFor function

class AnnoyingHelper : public sc2::Agent {
public:
    bool CanPathToLocation(sc2::Tag tag, sc2::Point2D& target_pos) {
        // Send a pathing query from the unit to that point. Can also query from point to point,
        // but using a unit tag wherever possible will be more accurate.
        // Note: This query must communicate with the game to get a result which affects performance.
        // Ideally batch up the queries (using PathingDistanceBatched) and do many at once.
        float distance = Query()->PathingDistance(tag, target_pos);

        return distance > 0.1f;
    }

    void TryMoveRandomUnit() {
        const sc2::ObservationInterface* observation = Observation();
        sc2::ActionInterface* action = Actions();

        sc2::Units my_units = observation->GetUnits(sc2::Unit::Alliance::Self);
        if (my_units.empty()) {
            return;
        }

        const sc2::Unit& unit = sc2::GetRandomEntry(my_units);

        sc2::Point2D move_target = sc2::FindRandomLocation(observation->GetGameInfo());
        if (!CanPathToLocation(unit.tag, move_target)) {
            return;
        }

        action->UnitCommand(unit, sc2::ABILITY_ID::SMART, move_target);
    }

    virtual void OnStep() final {
        TryMoveRandomUnit();
    }
};

//*************************************************************************************************
int main(int argc, char* argv[]) {
    sc2::Coordinator coordinator;
    if (!coordinator.LoadSettings(argc, argv)) {
        return 1;
    }

    coordinator.SetRealtime(true);

    // Add the helper, it will annoy the players.
    AnnoyingHelper bot;

    coordinator.SetParticipants({
        CreateParticipant(sc2::Race::Terran, &bot),
        CreateComputer(sc2::Race::Zerg)
    });

    // Start the game.
    coordinator.LaunchStarcraft();
    coordinator.StartGame(sc2::kMapBelShirVestigeLE);

    // Step forward the game simulation.
    while (coordinator.Update()) {
        sc2::SleepFor(5 * 1000);
    }

    return 0;
}