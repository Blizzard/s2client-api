#include "sc2api/sc2_api.h"
#include "sc2lib/sc2_lib.h"
#include "sc2utils/sc2_manage_process.h"

#include <iostream>
//#include "qlbot/bot_zealot.h"
//#include "qlbot/bot_marine.h"
#include "qlbot/bot_ql_test.h"

class Human : public sc2::Agent {


};

class FooBot : public sc2::Agent {
public:
    uint32_t restarts_;

    FooBot() :
        restarts_(0) {
    }

    virtual void OnGameStart() final {
        std::cout << "Starting a new game (" << restarts_ << " restarts)" << std::endl;

    };

    virtual void OnStep() final {
        uint32_t game_loop = Observation()->GetGameLoop();

        /*if (game_loop % 100 == 0) {
        sc2::Units units = Observation()->GetUnits(sc2::Unit::Alliance::Self);
        for (auto& it_unit : units) {
        sc2::Point2D target = sc2::FindRandomLocation(Observation()->GetGameInfo());
        Actions()->UnitCommand(it_unit, sc2::ABILITY_ID::ATTACK, target);
        }
        sc2::Units units = Observation()->GetUnits(sc2::Unit::Alliance::Self);
        for (auto& it_unit : units) {
        sc2::Point2D target = sc2::FindRandomLocation(Observation()->GetGameInfo());
        Actions()->UnitCommand(it_unit, sc2::ABILITY_ID::SMART, target);
        }
        }*/
    };

    virtual void OnGameEnd() final {
        ++restarts_;
        std::cout << "Game ended after: " << Observation()->GetGameLoop() << " loops " << std::endl;
    };

private:
};

//*************************************************************************************************
int main(int argc, char* argv[]) {
    sc2::Coordinator coordinator;
    if (!coordinator.LoadSettings(argc, argv)) {
        return 1;
    }
    coordinator.SetMultithreaded(true);
    // Add the custom bot, it will control the players.
    //ZealotBot zealot;
    QlBot* tester = new QlBot(7, 7, 2.0);
    //MarineBot marine;
    Human human;

    coordinator.SetParticipants({
        CreateParticipant(sc2::Race::Protoss, tester)
        });
    coordinator.SetRealtime(false);
    // Start the game.
    coordinator.LaunchStarcraft();
    std::cout << "TU MA BYT KOD PRE BOTA ZEALOTA, PUSTA sA MINIHRA JAMA" << std::endl;
    // Step forward the game simulation.
    bool do_break = false;
    while (!do_break) {
        coordinator.StartGame(sc2::kMapEmptyLong);
        while (coordinator.Update() && !do_break) {
            if (sc2::PollKeyPress()) {
                std::cout << "Koncim cyklus" << std::endl;
                do_break = true;
            }
        }
    }

    return 0;
}
