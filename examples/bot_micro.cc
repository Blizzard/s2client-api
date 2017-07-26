#include "sc2api/sc2_api.h"

#include "sc2utils/sc2_arg_parser.h"
#include "sc2utils/sc2_manage_process.h"

#include "bot_examples.h"

#include <limits>
#include <iostream>
#include <unordered_map>
#include <random>


static const bool UseRealtime = true;

//*************************************************************************************************
int main(int argc, char* argv[]) {
    sc2::Coordinator coordinator;
    if (!coordinator.LoadSettings(argc, argv)) {
        return 1;
    }

    coordinator.SetRealtime(true);

    // Add the custom bot, it will control the player.
    sc2::MarineMicroBot bot;
    coordinator.SetParticipants({
        CreateParticipant(sc2::Race::Terran, &bot),
        CreateComputer(sc2::Race::Zerg),
    });

    // Start the game.
    coordinator.LaunchStarcraft();
    coordinator.StartGame("MarineMicro.SC2Map");

    while (coordinator.Update()) {
        if (sc2::PollKeyPress()) {
            break;
        }
    }

    return 0;
}
