#include "sc2api/sc2_api.h"
#include "sc2utils/sc2_manage_process.h"

#include <iostream>
#include <string>
#include <random>


static const uint32_t ResetTarget = 100;

class FooBot : public sc2::Agent {
public:
    uint32_t last_game_loop_;
    uint32_t game_loop;
    uint32_t restarts_;
    sc2::GameInfo game_info_;

    FooBot() :
        restarts_(0),
        Agent() {
    }

    virtual void OnGameStart() final {
        last_game_loop_ = 0;
        game_info_ = Observation()->GetGameInfo();
        std::cout << "Starting a new game (" << std::to_string(restarts_) << " restarts)" << std::endl;
    };

    sc2::Point2D FindRandomLocation() {
        sc2::Point2D target_pos;
        float playable_w =  game_info_.playable_max.x - game_info_.playable_min.x;
        float playable_h =  game_info_.playable_max.y - game_info_.playable_min.y;
        target_pos.x = playable_w * sc2::GetRandomFraction() + game_info_.playable_min.x;
        target_pos.y = playable_h * sc2::GetRandomFraction() + game_info_.playable_min.y;
        return target_pos;
    }

    virtual void OnStep() final {
        const sc2::ObservationInterface* observation = Observation();
        game_loop = observation->GetGameLoop();

        if (ResetTarget < game_loop - last_game_loop_) {
            last_game_loop_ = game_loop;
            sc2::Units units = Observation()->GetUnits(sc2::Unit::Alliance::Self);
            for (auto& it_unit : units) {
                Actions()->UnitCommand(it_unit, 1, FindRandomLocation());
            }
        }
    };

    virtual void OnGameEnd() final {
        ++restarts_;
        std::cout << "Game ended after: " << std::to_string(game_loop) << " loops " << std::endl;
    };

private:
};

//*************************************************************************************************
int main(int argc, char* argv[]) {
    sc2::Coordinator coordinator;
    if (!coordinator.LoadSettings(argc, argv)) {
        return 1;
    }

    // Add the custom bot, it will control the players.
    FooBot bot;

    coordinator.SetParticipants({
        CreateParticipant(sc2::Race::Terran, &bot),
        CreateComputer(sc2::Race::Terran)
    });

    // Start the game.
    coordinator.LaunchStarcraft();

    // Step forward the game simulation.
    bool do_break = false;
    while (!do_break) {
        coordinator.StartGame(sc2::GetMapPath(sc2::Map::BelShirVestigeLE));
        while (coordinator.Update() && !do_break) {
            if (sc2::PollKeyPress()) {
                do_break = true;
            }
        }
    }

    return 0;
}
