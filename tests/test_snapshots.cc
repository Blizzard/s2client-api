#include "test_framework.h"
#include "test_movement_combat.h"
#include "sc2api/sc2_api.h"
#include <iostream>
#include <string>
#include <random>
#include "feature_layers_shared.h"

namespace sc2 {

//
// SnapshotTestBot
//

class SnapshotTestBot : public UnitTestBot {
public:
    Point2D base_pos_;
    Point2D mineral_pos_;
    Point2D scv_pos_;
    Tag mineral_tag_;

    SnapshotTestBot();

    bool IsNearPos(const Point2D& a, const Point2D& b) const {
        float d = Distance2D(a, b);
        return d < 0.5f;
    }
private:
    void OnTestsBegin () final {};
    void OnTestsEnd () final {};
};

class TestSnapshotBase : public TestSequence {
public:
    SnapshotTestBot* bot_;

    TestSnapshotBase() :
        bot_(nullptr) {
    }

    TestSnapshotBase(SnapshotTestBot* bot) :
        bot_(bot) {
    }

    bool FindSCV(Unit& unit) {
        assert(bot_);
        Units my_scvs = agent_->Observation()->GetUnits(Unit::Self, [](const Unit& unit){
                return unit.unit_type == UNIT_TYPEID::TERRAN_SCV;
        });
        if (my_scvs.size() < 1) {
            ReportError("Can't find a friendly TERRAN_SCV!");
            assert(0);
            return false;
        }

        unit = my_scvs[0];
        return true;
    }
};

#define TEST_SNAPSHOT_CLASS(X)      \
    X() :                           \
        TestSnapshotBase() {         \
    }                               \
    X(SnapshotTestBot* bot) :       \
        TestSnapshotBase(bot) {      \
    }

class TestSnapshot1 : public TestSnapshotBase {
public:
    TEST_SNAPSHOT_CLASS(TestSnapshot1)

    void OnTestStart() override {
        assert(bot_);
        wait_game_loops_ = 10;
        Point2D playable_min = agent_->Observation()->GetGameInfo().playable_min;
        Point2D playable_max = agent_->Observation()->GetGameInfo().playable_max;
        Point2D center = (playable_max - playable_min) * 0.5f + playable_min;

        uint32_t player_id = agent_->Observation()->GetPlayerID();

        // Build a command center and an TERRAN_SCV nearby.
        Point2D base = center;
        //base.x_ += (playable_max.x_ - center.x_) * 0.5f;
        agent_->Debug()->DebugCreateUnit(UNIT_TYPEID::TERRAN_COMMANDCENTER, base, player_id, 1);
        Point2D scv_pt = base;
        scv_pt.y += 4.0f;
        agent_->Debug()->DebugCreateUnit(UNIT_TYPEID::TERRAN_SCV, scv_pt, player_id, 1);

        // Build a mineral patch.
        Point2D mineral_pt = base - Point2D(12.0f, -12.0f);
        agent_->Debug()->DebugCreateUnit(UNIT_TYPEID::NEUTRAL_MINERALFIELD, mineral_pt, 0, 1);

        // Create the units.
        agent_->Debug()->DebugShowMap();
        agent_->Debug()->SendDebug();

        // Center the camera.
        Point2D midpoint = (mineral_pt + base) * 0.5f;
        Point2DI minimap_pos = ConvertWorldToMinimap(agent_->Observation()->GetGameInfo(), midpoint);
        ActionFeatureLayerInterface* action = agent_->ActionsFeatureLayer();
        action->CameraMove(minimap_pos);

        bot_->base_pos_ = base;
        bot_->mineral_pos_ = mineral_pt;
    }
};

class TestSnapshot2 : public TestSnapshotBase {
public:
    TEST_SNAPSHOT_CLASS(TestSnapshot2)

    void OnTestStart() override {
        assert(bot_);
        wait_game_loops_ = 10;

        // Get the tag for the mineral patch.
        Units minerals = agent_->Observation()->GetUnits(Unit::Neutral, [](const Unit& unit){
                return unit.unit_type == UNIT_TYPEID::NEUTRAL_MINERALFIELD;
        });
        if (minerals.size() < 1) {
            ReportError("Can't find the mineral patch!");
            assert(0);
            return;
        }
        bot_->mineral_tag_ = minerals[0].tag;

        agent_->Debug()->DebugShowMap();
        agent_->Debug()->SendDebug();
    }
};

class TestSnapshot3 : public TestSnapshotBase {
public:
    TEST_SNAPSHOT_CLASS(TestSnapshot3)

    void OnTestStart() override {
        assert(bot_);
        wait_game_loops_ = 10;

        agent_->Debug()->DebugKillUnit(bot_->mineral_tag_);
        agent_->Debug()->SendDebug();

        const ObservationInterface* obs = agent_->Observation();
        ActionFeatureLayerInterface* actions = agent_->ActionsFeatureLayer();

        Point2D camera_world = obs->GetCameraPos();

        Unit unit_scv;
        if (!FindSCV(unit_scv)) {
            return;
        }

        bot_->scv_pos_ = unit_scv.pos;
        Point2DI scv_pt = ConvertWorldToCamera(obs->GetGameInfo(), camera_world, bot_->scv_pos_);
        actions->Select(scv_pt, PointSelectionType::PtSelect);
        std::cout << "Selecting an scv at world pt: " << std::to_string(unit_scv.pos.x) << ", " << std::to_string(unit_scv.pos.y) << std::endl;
        std::cout << "Selecting an scv at screen pt: " << std::to_string(scv_pt.x) << ", " << std::to_string(scv_pt.y) << std::endl;
    }
};

class TestSnapshot4 : public TestSnapshotBase {
public:
    TEST_SNAPSHOT_CLASS(TestSnapshot4)

    void OnTestStart() override {
        assert(bot_);
        wait_game_loops_ = 20;
        const ObservationInterface* obs = agent_->Observation();
        ActionFeatureLayerInterface* actions = agent_->ActionsFeatureLayer();
        Point2D camera_world = obs->GetCameraPos();

        Point2D empty_pos = bot_->mineral_pos_;
        empty_pos.y += 5;
        Point2DI empty_pos_screen = ConvertWorldToCamera(agent_->Observation()->GetGameInfo(), camera_world, empty_pos);
        actions->UnitCommand(ABILITY_ID::HARVEST_GATHER, empty_pos_screen);
    }

    void OnTestFinish() override {
        Unit unit_scv;
        if (!FindSCV(unit_scv)) {
            return;
        }

        if (!bot_->IsNearPos(bot_->scv_pos_, unit_scv.pos)) {
            ReportError("The TERRAN_SCV should not have moved!");
            assert(0);
        }
    }
};

class TestSnapshot5 : public TestSnapshotBase {
public:
    TEST_SNAPSHOT_CLASS(TestSnapshot5)

    void OnTestStart() override {
        assert(bot_);
        wait_game_loops_ = 20;
        const ObservationInterface* obs = agent_->Observation();
        ActionFeatureLayerInterface* actions = agent_->ActionsFeatureLayer();
        Point2D camera_world = obs->GetCameraPos();
        Point2DI mineral_pos = ConvertWorldToCamera(agent_->Observation()->GetGameInfo(), camera_world, bot_->mineral_pos_);

        actions->UnitCommand(ABILITY_ID::HARVEST_GATHER, mineral_pos);
    }

    void OnTestFinish() override {
        Unit unit_scv;
        if (!FindSCV(unit_scv)) {
            return;
        }

        if (bot_->IsNearPos(bot_->scv_pos_, unit_scv.pos)) {
            ReportError("The TERRAN_SCV should have moved!");
            assert(0);
        }
    }
};

class TestSnapshot6 : public TestSnapshotBase {
public:
    TEST_SNAPSHOT_CLASS(TestSnapshot6)

    void OnTestStart() override {
        assert(bot_);
        wait_game_loops_ = 20;
    }
};


//
// SnapshotTestBot implementation.
//

SnapshotTestBot::SnapshotTestBot() :
    mineral_tag_(NullTag) {
    Add(TestSnapshot1(this));    // Spawn TERRAN_SCV, mineral field and command center.
    Add(TestSnapshot2(this));    // Get the mineral field tag and toggle off vision.
    Add(TestSnapshot3(this));    // Destroy the mineral field and select the TERRAN_SCV.
    Add(TestSnapshot4(this));    // Target something other than the mineral field. TERRAN_SCV should not move.
    Add(TestSnapshot5(this));    // Target the mineral field. TERRAN_SCV should move.
    Add(TestSnapshot6(this));
}

//
// TestFeatureLayers
//

bool TestSnapshots(int argc, char** argv) {
    Coordinator coordinator;
    if (!coordinator.LoadSettings(argc, argv)) {
        return false;
    }
    coordinator.SetFeatureLayers(FeatureLayerSettings());

    // Add the custom bot, it will control the players.
    SnapshotTestBot bot;

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

