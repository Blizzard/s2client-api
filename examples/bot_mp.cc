#include "sc2api/sc2_api.h"
#include "sc2utils/sc2_manage_process.h"

#include "bot_examples.h"

#include <iostream>


// Set this flag to true if you want to play against your bot.
static bool PlayerOneIsHuman = false;

class Human : public sc2::Agent {
public:
    void OnGameStart() final {
        Debug()->DebugTextOut("Human");
        Debug()->SendDebug();
    }
};

int main(int argc, char* argv[]) {
    sc2::Coordinator coordinator;
    if (!coordinator.LoadSettings(argc, argv)) {
        return 1;
    }

    coordinator.SetMultithreaded(true);
    if (PlayerOneIsHuman) {
        coordinator.SetRealtime(true);
    }

    // Add the custom bot, it will control the players.
    sc2::TerranBot bot1, bot2;
    Human human_bot;

    sc2::Agent* player_one = &bot1;
    if (PlayerOneIsHuman) {
        player_one = &human_bot;
    }

    coordinator.SetParticipants({
        CreateParticipant(sc2::Race::Terran, player_one),
        CreateParticipant(sc2::Race::Terran, &bot2),
    });

    // Start the game.
    coordinator.LaunchStarcraft();

    bool do_break = false;
    while (!do_break) {
        if (!coordinator.StartGame(sc2::kMapBelShirVestigeLE)) {
            break;
        }
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
