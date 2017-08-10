#include "test_framework.h"
#include "test_movement_combat.h"
#include "sc2api/sc2_api.h"
#include <iostream>
#include <string>
#include <random>
#include "feature_layers_shared.h"

namespace sc2 {

class TestAppBasic : public TestSequence {
public:
    DebugInterface::AppTest app_test_;
    int delay_ms_;

    TestAppBasic() :
        TestSequence() {
    }

    TestAppBasic(DebugInterface::AppTest app_test, int delay_ms = 0) :
        app_test_(app_test),
        delay_ms_(delay_ms) {
    }

    void OnTestStart() override {
        wait_game_loops_ = 10;
    }

    void OnTestFinish() override {
        DebugInterface* debug = agent_->Debug();
        debug->DebugTestApp(app_test_, delay_ms_);
        debug->SendDebug();
    }
};

class AppTestBotHang : public UnitTestBot {
public:
    bool reported_hang_;

    AppTestBotHang() :
        reported_hang_(false) {
        Add(TestAppBasic(DebugInterface::hang));
    }

    void OnTestsBegin() override {
    }

    void OnTestsEnd() override {
    }

    void OnError(const std::vector<ClientError>& client_errors, const std::vector<std::string>& protocol_errors = {}) override {
        for (auto e : client_errors) {
            if (e == ClientError::SC2ProtocolTimeout) {
                reported_hang_ = true;
                UnitTestBot::OnError(client_errors, protocol_errors);
            }
        }
    }
};

class AppTestBotCrash : public UnitTestBot {
public:
    bool reported_crash_;

    AppTestBotCrash() :
        reported_crash_(false) {
        Add(TestAppBasic(DebugInterface::crash));
    }

    void OnTestsBegin() override {
    }

    void OnTestsEnd() override {
    }

    void OnError(const std::vector<ClientError>& client_errors, const std::vector<std::string>& protocol_errors = {}) override {
        for (auto e : client_errors) {
            if (e == ClientError::SC2AppFailure) {
                reported_crash_ = true;
                UnitTestBot::OnError(client_errors, protocol_errors);
            }
        }
    }
};

bool TestHang(int argc, char** argv) {
    Coordinator coordinator;
    if (!coordinator.LoadSettings(argc, argv)) {
        return false;
    }

    // Add the custom bot, it will control the players.
    AppTestBotHang bot1;

    coordinator.SetParticipants({
        CreateParticipant(sc2::Race::Terran, &bot1),
    });

    // Start the game.
    coordinator.LaunchStarcraft();
    coordinator.StartGame(sc2::kMapEmpty);

    // Step forward the game simulation.
    while (!bot1.IsFinished()) {
        coordinator.Update();
    }

    return bot1.reported_hang_;
}

bool TestCrash(int argc, char** argv) {
    Coordinator coordinator;
    if (!coordinator.LoadSettings(argc, argv)) {
        return false;
    }

    // Add the custom bot, it will control the players.
    AppTestBotCrash bot1;

    coordinator.SetParticipants({
        CreateParticipant(sc2::Race::Terran, &bot1),
    });

    // Start the game.
    coordinator.LaunchStarcraft();
    coordinator.StartGame(sc2::kMapEmpty);

    // Step forward the game simulation.
    while (!bot1.IsFinished()) {
        coordinator.Update();
    }

    return bot1.reported_crash_;
}


//
// TestFeatureLayers
//

bool TestApp(int argc, char** argv) {
    if (!TestCrash(argc, argv))
        return false;
    if (!TestHang(argc, argv))
        return false;

    return true;
}

}

