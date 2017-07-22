#include "sc2api/sc2_api.h"
#include "test_framework.h"
#include "feature_layers_shared.h"

#include <iostream>

namespace sc2 {

class SpawnZerglings : public TestSequence {
public:
    void OnTestStart() override {
        agent_->Debug()->DebugMoveCamera(sc2::Point2D(35.0f, 35.0f));
        agent_->Debug()->DebugCreateUnit(UNIT_TYPEID::ZERG_ZERGLING, sc2::Point2D(30.0f, 30.0f));
        agent_->Debug()->DebugCreateUnit(UNIT_TYPEID::ZERG_ZERGLING, sc2::Point2D(40.0f, 40.0f));
        agent_->Debug()->SendDebug();
    }
};

class PatrolZerglings : public TestSequence {
public:
    void OnTestStart() override {
        const ObservationInterface* obs = agent_->Observation();
        ActionInterface* act = agent_->Actions();

        for (const Unit& unit : obs->GetUnits()) {
            sc2::Point2D target(
               (unit.pos.x < 35.0f) ? 40.0f : 30.0f,
                unit.pos.y
            );

            act->UnitCommand(unit, ABILITY_ID::PATROL, target);
        }
    }
};

class SelectZergling : public TestSequence {
public:
    Point2DI target;

    void OnTestStart() override {
        wait_game_loops_ = 1;

        const ObservationInterface* obs = agent_->Observation();
        ActionFeatureLayerInterface* act = agent_->ActionsFeatureLayer();

        for (const Unit& unit : obs->GetUnits()) {
            if (unit.is_selected)
                continue;

            target = ConvertWorldToCamera(obs->GetGameInfo(), obs->GetCameraPos(), unit.pos);
            act->Select(target, PointSelectionType::PtSelect);
            return;
        }

        ReportError("Failed to find unselected zergling");
    }

    void OnTestFinish() override {
        const ObservationInterface* obs = agent_->Observation();
        const SpatialActions& actions = obs->GetFeatureLayerActions();

        if (actions.select_points.size() != 1) {
            ReportError("Missing action");
            return;
        }

        const Point2DI& point = actions.select_points[0].select_screen;
        if (point != target) {
            ReportError("Action is targeting wrong point");
            return;
        }
    }
};

//
// ActionTestBot
//

class ActionTestBot : public UnitTestBot {
public:
    ActionTestBot();

private:
    void OnTestsBegin () final;
    void OnTestsEnd () final;
};

ActionTestBot::ActionTestBot() :
    UnitTestBot() {
    // Sequences.
    Add(SpawnZerglings());
    Add(PatrolZerglings());
    for (int i = 0; i < 50; ++i)
        Add(SelectZergling());
}

void ActionTestBot::OnTestsBegin() {

};

void ActionTestBot::OnTestsEnd () {

}


//
// TestObservationActions
//

bool TestObservationActions(int argc, char** argv) {
    Coordinator coordinator;
    if (!coordinator.LoadSettings(argc, argv)) {
        return false;
    }

    coordinator.SetFeatureLayers(FeatureLayerSettings());

    // Add the custom bot, it will control the players.
    ActionTestBot bot;

    coordinator.SetParticipants({
        CreateParticipant(sc2::Race::Terran, &bot),
    });

    // Start the game.
    coordinator.LaunchStarcraft();
    coordinator.StartGame(sc2::kMapEmpty);

    // Step forward the game simulation.
    while (!bot.IsFinished()) {
        coordinator.Update();
    }

    return bot.Success();
}

}

