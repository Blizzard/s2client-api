#include "test_framework.h"
#include "test_movement_combat.h"
#include "test_common.h"
#include "sc2api/sc2_api.h"
#include <iostream>
#include <string>
#include <random>
#include "feature_layers_shared.h"

namespace sc2 {

//
// TestLayerUpdate
//

class TestLayerUpdate : public TestSequence {
public:
    FeatureLayer8BPP start_;
    Point2D camera_start_;

    void OnTestStart() override {
        const char* error = GetPlayerRelativeLayer(agent_, FeatureLayerType::MINIMAP, start_);
        if (error) {
            ReportError(error);
            return;
        }

        const ObservationInterface* obs = agent_->Observation();
        camera_start_ = obs->GetCameraPos();

        const GameInfo& game_info = obs->GetGameInfo();
        Point2D center = FindCenterOfMap(game_info);
        Point2DI center_minimap = ConvertWorldToMinimap(game_info, center);

        ActionFeatureLayerInterface* action = agent_->ActionsFeatureLayer();
        action->CameraMove(center_minimap);
 
        DebugInterface* debug = agent_->Debug();
        debug->DebugCreateUnit(UNIT_TYPEID::TERRAN_MARINE, center, agent_->Observation()->GetPlayerID(), 1);
        debug->SendDebug();

        wait_game_loops_ = 5;
    }

    void OnStep() override {

    }

    void OnTestFinish() override {
        const ObservationInterface* obs = agent_->Observation();
        Point2D camera_end = obs->GetCameraPos();
        if (camera_start_ == camera_end) {
            ReportError("Camera didn't move");
            return;
        }

        FeatureLayer8BPP end_;
        const char* error = GetPlayerRelativeLayer(agent_, FeatureLayerType::MINIMAP, end_);
        if (error) {
            ReportError(error);
            return;
        }

        if (start_.width != end_.width || start_.height != end_.height) {
            ReportError("Layers not the same size");
            return;
        }

        bool changed = false;
        for (int x = 0; x < start_.width; ++x) {
            for (int y = 0; y < start_.height; ++y) {
                char start_value = start_.Read(Point2DI(x, y));
                char end_value = end_.Read(Point2DI(x, y));
                if (start_value != end_value)
                    changed = true;
            }
        }

        if (!changed) {
            ReportError("Layers didn't update");
            return;
        }
    }
};


//
// FeatureLayerMPTestBot
//

class FeatureLayerMPTestBot : public UnitTestBot {
public:
    FeatureLayerMPTestBot();

private:
    void OnTestsBegin () final;
    void OnTestsEnd () final;
};

FeatureLayerMPTestBot::FeatureLayerMPTestBot() :
    UnitTestBot() {
    // Sequences.
    Add(TestLayerUpdate());
}

void FeatureLayerMPTestBot::OnTestsBegin() {
    Debug()->DebugShowMap();
};

void FeatureLayerMPTestBot::OnTestsEnd () {

}


//
// TestFeatureLayersMP
//

bool TestFeatureLayersMP(int argc, char** argv) {
    Coordinator coordinator;
    if (!coordinator.LoadSettings(argc, argv)) {
        return false;
    }

    coordinator.SetFeatureLayers(FeatureLayerSettings());

    // Add the custom bot, it will control the players.
    FeatureLayerMPTestBot bot1;
    FeatureLayerMPTestBot bot2;

    coordinator.SetParticipants({
        CreateParticipant(sc2::Race::Terran, &bot1),
        CreateParticipant(sc2::Race::Terran, &bot2),
    });

    // Start the game.
    coordinator.LaunchStarcraft();
    coordinator.StartGame(GetMapPath(Map::BelShirVestigeLE));

    // Step forward the game simulation.
    while (!bot1.IsFinished() && !bot2.IsFinished()) {
        coordinator.Update();
    }

    return bot1.Success() && bot2.Success();
}

}

