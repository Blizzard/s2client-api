#include "sc2api/sc2_api.h"

#include "sc2utils/sc2_manage_process.h"

#include <iostream>
#include <string>
#include <random>
#include <cmath>


// a helper bot that actually just moves a random unit every 5 seconds
// timing is controlled in Main, with the SleepFor function

class AnnoyingHelper : public sc2::Agent {
public:

    sc2::GameInfo game_info_;

    virtual void OnGameStart() {
        game_info_ = Observation()->GetGameInfo();
    }

    bool TryFindRandomPathableLocation(sc2::Tag tag, sc2::Point2D& target_pos) {
        // First, find a random point inside the playable area of the map.
        float playable_w = game_info_.playable_max.x - game_info_.playable_min.x;
        float playable_h = game_info_.playable_max.y - game_info_.playable_min.y;

        // The case where game_info_ does not provide a valid answer
        if (playable_w == 0 || playable_h == 0) {
            playable_w = 236;
            playable_h = 228;
        }

        target_pos.x = playable_w * sc2::GetRandomFraction() + game_info_.playable_min.x;
        target_pos.y = playable_h * sc2::GetRandomFraction() + game_info_.playable_min.y;

        // Now send a pathing query from the unit to that point. Can also query from point to point,
        // but using a unit tag wherever possible will be more accurate.
        // Note: This query must communicate with the game to get a result which affects performance.
        // Ideally batch up the queries (using PathingDistanceBatched) and do many at once.
        float distance = Query()->PathingDistance(tag, target_pos);

        return distance > 0.1f;
    }

    bool GetRandomUnit(sc2::Unit& unit_out, const sc2::ObservationInterface* observation) {
        int count = 0;
        sc2::Units my_units = observation->GetUnits(sc2::Unit::Alliance::Self);
        if (!my_units.empty()) {
            unit_out = GetRandomEntry(my_units);
            return true;
        }
        return false;
    }

    bool TryMoveRandomUnit(const sc2::ObservationInterface* observation) {
        sc2::Unit unit;
        sc2::Point2D move_target;
        sc2::ActionInterface* action = Actions();

        if (!GetRandomUnit(unit, observation)) {
            return false;
        }

        if (!TryFindRandomPathableLocation(unit.tag, move_target)) {
            return false;
        }

        action->UnitCommand(unit, sc2::ABILITY_ID::SMART, move_target);

        return true;
    }

    virtual void OnStep() final {
        const sc2::ObservationInterface* observation = Observation();

        TryMoveRandomUnit(observation);
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
    coordinator.StartGame(sc2::GetMapPath(sc2::Map::BelShirVestigeLE));

    // Step forward the game simulation.
    while (coordinator.Update()) {
        sc2::SleepFor(5 * 1000);
    }

    return 0;
}