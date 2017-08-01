#include "sc2api/sc2_api.h"
#include "sc2utils/sc2_manage_process.h"

#include "bot_examples.h"

#include <iostream>

int main(int argc, char* argv[]) {
    sc2::Coordinator coordinator;
    if (!coordinator.LoadSettings(argc, argv)) {
        return 1;
    }

    coordinator.SetMultithreaded(true);
    // Add the custom bot, it will control the players.
    sc2::ProtossMultiplayerBot bot1;
    sc2::ZergMultiplayerBot bot2;
    sc2::TerranMultiplayerBot bot3;

    coordinator.SetParticipants({
        CreateParticipant(sc2::Race::Protoss, &bot1),
        CreateComputer(sc2::Race::Terran,sc2::Difficulty::Hard)
    });

    // Start the game.
    coordinator.LaunchStarcraft();
    bool do_break = false;

    for (; !do_break;) {
        coordinator.StartGame(sc2::kMapBelShirVestigeLE);
        //bot1.air_build_ = !bot1.air_build_;
        bot1.air_build_ = false;
        bot2.mutalisk_build_ = false;

        while (coordinator.Update() && !do_break) {
            if (sc2::PollKeyPress()) {
                do_break = true;
            }
        }
    }
    bot1.Control()->DumpProtoUsage();
    bot2.Control()->DumpProtoUsage();
    return 0;
}