#include "test_framework.h"
#include "test_movement_combat.h"
#include "sc2api/sc2_api.h"
#include "sc2utils/sc2_manage_process.h"
#include <iostream>
#include <string>
#include <random>
#include "feature_layers_shared.h"

namespace sc2 {

//
// TestRemapStart
//

class TestRemap : public TestSequence {
public:
    Point2DI command_center_pt_;

    TestRemap() :
        command_center_pt_(24, 24) {
    }

    Unit GetCommandCenter() {
        const ObservationInterface* obs = agent_->Observation();
        Units found_command_centers = obs->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::TERRAN_COMMANDCENTER));
        if (found_command_centers.size() != 1) {
            ReportError("Could not find the command center");
            assert(0);
            return Unit();
        }

        return found_command_centers[0];
    }
};

class TestRemapStart : public TestRemap {
public:
    void OnTestStart() override {
        const GameInfo& game_info = agent_->Observation()->GetGameInfo();

        Point2D min = game_info.playable_min;
        Point2D max = game_info.playable_max;
        Point2D spawn_pt = min + (max - min) / 2.0f;

        agent_->Debug()->DebugCreateUnit(UNIT_TYPEID::TERRAN_SCV, spawn_pt, agent_->Observation()->GetPlayerID(), 1);
        agent_->Debug()->DebugMoveCamera(spawn_pt);
        agent_->Debug()->DebugGiveAllResources();
        agent_->Debug()->DebugIgnoreMineral();
        agent_->Debug()->SendDebug();
    }
};

class TestRemapSelectSCV : public TestRemap {
public:
    void OnTestStart() override {
        ActionFeatureLayerInterface* action = agent_->ActionsFeatureLayer();
        Point2DI p0(0, 0);
        Point2DI p1(63, 63);
        action->Select(p0, p1);
    }
};

class TestRemapBuildCommandCenter : public TestRemap {
public:
    void OnTestStart() override {
        ActionFeatureLayerInterface* action = agent_->ActionsFeatureLayer();
        action->UnitCommand(ABILITY_ID::BUILD_COMMANDCENTER, command_center_pt_);
        wait_game_loops_ = 50;
    }
};

class TestRemapBuildCommandCenterHalt : public TestRemap {
public:
    void OnTestStart() override {
        ActionFeatureLayerInterface* action = agent_->ActionsFeatureLayer();
        action->UnitCommand(ABILITY_ID::HALT);
        wait_game_loops_ = 10;
    }
};

class TestRemapBuildCommandCenterDetectHalt : public TestRemap {
public:
    float current_progress;

    void OnTestStart() override {
        Unit command_center = GetCommandCenter();
        current_progress = command_center.build_progress;
        if (current_progress == 0.0f) {
            ReportError("Command center did not start building");
            assert(0);
            return;
        }
        if (current_progress == 1.0f) {
            ReportError("Command center finished building");
            assert(0);
            return;
        }
        wait_game_loops_ = 50;
    }

    void OnTestFinish() override {
        Unit command_center = GetCommandCenter();
        if (current_progress != command_center.build_progress) {
            ReportError("Command center building continued");
            assert(0);
            return;
        }
    }
};

class TestRemapBuildCommandCenterContinue : public TestRemap {
public:
    float current_progress;

    void OnTestStart() override {
        Unit command_center = GetCommandCenter();
        current_progress = command_center.build_progress;
        ActionFeatureLayerInterface* action = agent_->ActionsFeatureLayer();
        action->UnitCommand(ABILITY_ID::SMART, command_center_pt_);
        wait_game_loops_ = 50;
    }

    void OnTestFinish() override {
        Unit command_center = GetCommandCenter();
        if (current_progress == command_center.build_progress) {
            ReportError("Command center building would not continue");
            assert(0);
            return;
        }

        // Select the barracks.
        ActionFeatureLayerInterface* action = agent_->ActionsFeatureLayer();
        action->Select(command_center_pt_, PointSelectionType::PtSelect);
    }
};

class TestRemapBuildCommandCenterCancel : public TestRemap {
public:
    void OnTestStart() override {
        ActionFeatureLayerInterface* action = agent_->ActionsFeatureLayer();
        action->UnitCommand(ABILITY_ID::CANCEL);
        wait_game_loops_ = 10;
    }

    void OnTestFinish() override {
        const ObservationInterface* obs = agent_->Observation();
        Units found_command_centers = obs->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::TERRAN_COMMANDCENTER));
        if (found_command_centers.size() != 0) {
            ReportError("Command center was not destroyed!");
            assert(0);
        }
    }
};

class TestRemapFinishTest : public TestRemap {
public:
    void OnTestStart() override {
        wait_game_loops_ = 5;
    }
};


//
// RemapAbilitiesTestBot
//

class RemapAbilitiesTestBot : public UnitTestBot {
public:
    RemapAbilitiesTestBot();

private:
    void OnTestsBegin () final;
    void OnTestsEnd () final;
};

RemapAbilitiesTestBot::RemapAbilitiesTestBot() :
    UnitTestBot() {
    // Sequences.
    Add(TestRemapStart());
    Add(TestRemapSelectSCV());
    Add(TestRemapBuildCommandCenter());
    Add(TestRemapBuildCommandCenterHalt());
    Add(TestRemapBuildCommandCenterDetectHalt());
    Add(TestRemapBuildCommandCenterContinue());
    Add(WaitT<5>());
    Add(TestRemapBuildCommandCenterHalt());
    Add(TestRemapBuildCommandCenterDetectHalt());
    Add(TestRemapBuildCommandCenterCancel());
    Add(TestRemapFinishTest());
}

void RemapAbilitiesTestBot::OnTestsBegin() {
    Debug()->DebugShowMap();
};

void RemapAbilitiesTestBot::OnTestsEnd () {

}


//
// TestFeatureLayers
//

bool TestAbilityRemap(int argc, char** argv) {
    Coordinator coordinator;
    if (!coordinator.LoadSettings(argc, argv)) {
        return false;
    }

    coordinator.SetFeatureLayers(sc2::FeatureLayerSettings());

    // Add the custom bot, it will control the players.
    RemapAbilitiesTestBot bot;

    coordinator.SetParticipants({
        CreateParticipant(sc2::Race::Terran, &bot),
    });

    // Start the game.
    coordinator.LaunchStarcraft();
    coordinator.StartGame(sc2::kMapEmpty);

    // Step forward the game simulation.
    while (!bot.IsFinished()) {
        sc2::SleepFor(20);
        coordinator.Update();
    }

    return bot.Success();
}

}

