#include "test_movement_combat.h"
#include "sc2api/sc2_api.h"
#include "sc2utils/sc2_manage_process.h"
#include <iostream>
#include <fstream>
#include <string>
#include <random>

namespace sc2 {


//
// MultiplayerTestBot
//

class MultiplayerTestBot : public UnitTestBot {
public:
    MultiplayerTestBot();

private:
    void OnTestsBegin () final;
    void OnTestsEnd () final;
};

MultiplayerTestBot::MultiplayerTestBot() :
    UnitTestBot() {
    Add(WaitT<500>());
}

void MultiplayerTestBot::OnTestsBegin() {
    Debug()->DebugShowMap();
    Debug()->DebugEnemyControl();
};

void MultiplayerTestBot::OnTestsEnd() {
}

bool RemoteSaveMap(sc2::Coordinator& coordinator, std::string source_map, std::string remote_path) {
    source_map = GetLibraryMapsDirectory() + source_map;
    std::string buffer;
    std::ifstream inStream(source_map, std::ios::binary);
    inStream.seekg(0, std::ios_base::end);
    int data_size = int(inStream.tellg());
    if (data_size < 1)
        return false;

    buffer.resize(data_size);
    inStream.seekg(0);
    inStream.read(&buffer[0], data_size);
    inStream.close();

    return coordinator.RemoteSaveMap(&buffer[0], data_size, remote_path);
}


//
// TestMovementCombat
//

bool TestMultiplayer(int argc, char** argv) {
    static const char* remote_path = "temp_foo.SC2Map";

    sc2::Coordinator coordinator;
    if (!coordinator.LoadSettings(argc, argv)) {
        return false;
    }

    coordinator.SetMultithreaded(true);

    // Add the custom bot, it will control the players.
    MultiplayerTestBot bot1, bot2;

    coordinator.SetParticipants({
        CreateParticipant(sc2::Race::Terran, &bot1),
        CreateParticipant(sc2::Race::Terran, &bot2),
    });

    // Start the game.
    coordinator.LaunchStarcraft();

    if (!RemoteSaveMap(coordinator, sc2::kMapBelShirVestigeLE, remote_path))
        return false;

    coordinator.StartGame(remote_path);

    // Step forward the game simulation.
    while (!bot1.IsFinished() && !bot2.IsFinished()) {
        coordinator.Update();
    }

    return true;
}

}

