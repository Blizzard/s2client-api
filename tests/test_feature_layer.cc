#include "test_framework.h"
#include "test_movement_combat.h"
#include "sc2api/sc2_api.h"
#include "sc2lib/sc2_lib.h"
#include "feature_layers_shared.h"

#include <iostream>
#include <string>
#include <random>

namespace sc2 {

//
// TestBasic
//

class TestBasic : public TestSequence {
public:

    void OnTestStart() override {
        const GameInfo& game_info = agent_->Observation()->GetGameInfo();
        Point2D spawn_pt = FindRandomLocation(game_info);
        agent_->Debug()->DebugCreateUnit(UNIT_TYPEID::TERRAN_MARINE, spawn_pt, agent_->Observation()->GetPlayerID(), 1);
        agent_->Debug()->SendDebug();

        wait_game_loops_ = 5;
    }

    void OnStep() override {

    }

    void OnTestFinish() override {
        // Cleanup Test
        KillAllUnits();

        // Validate Result
        FeatureLayer8BPP data;
        const char* error = GetPlayerRelativeLayer(agent_, FeatureLayerType::MINIMAP, data);
        if (error) {
            ReportError(error);
            return;
        }

        bool foundUnit = false;
        for (int x = 0; x < data.width; ++x) {
            for (int y = 0; y < data.height; ++y) {
                char value = data.Read(Point2DI(x, y));
                if (value > 0) {
                    foundUnit = true;
                }
            }
        }

        if (!foundUnit)
            ReportError("Unit not found in feature layer");
    }
};


//
// TestCoordinateSystemMinimap
//

class TestCoordinateSystemMinimap : public TestSequence {
public:
    Point2D spawn_pt_;

    void OnTestStart() override {
        const GameInfo& mapInfo = agent_->Observation()->GetGameInfo();
        spawn_pt_= FindRandomLocation(mapInfo);
        agent_->Debug()->DebugCreateUnit(UNIT_TYPEID::TERRAN_MARINE, spawn_pt_, agent_->Observation()->GetPlayerID(), 1);
        agent_->Debug()->SendDebug();

        wait_game_loops_ = 5;
    }

    void OnStep() override {

    }

    void OnTestFinish() override {
        // Cleanup Test
        KillAllUnits();

        // Validate Result
        FeatureLayer8BPP data;
        const char* error = GetPlayerRelativeLayer(agent_, FeatureLayerType::MINIMAP, data);
        if (error) {
            ReportError(error);
            return;
        }

        const GameInfo& game_info = agent_->Observation()->GetGameInfo();
        Point2DI minimap_pos = ConvertWorldToMinimap(game_info, spawn_pt_);

        if (!data.InBounds(minimap_pos)) {
            ReportError("Unit not in minimap bounds");
            return;
        }

        if (data.Read(minimap_pos) == 0)
            ReportError("Unit not found in feature layer at expected location");
    }
};


//
// TestCoordinateSystemMap
//

class TestCoordinateSystemMap : public TestSequence {
public:
    Point2D spawn_pt_;

    void OnTestStart() override {
       const GameInfo& game_info = agent_->Observation()->GetGameInfo();
        float camera_size = game_info.options.feature_layer.camera_width;
        float max_spawn_offset = (camera_size / 2.0f) - 1.0f;

        Point2D min = game_info.playable_min;
        Point2D max = game_info.playable_max;
        min.x += max_spawn_offset;
        min.y += max_spawn_offset;
        max.x -= max_spawn_offset;
        max.y -= max_spawn_offset;
        if (min.x > max.x || min.y > max.y) {
            ReportError("Map is too small to run test");
            return;
        }

        Point2D camera_pt_= FindRandomLocation(min, max);
        spawn_pt_.x = camera_pt_.x + GetRandomScalar() * max_spawn_offset;
        spawn_pt_.y = camera_pt_.y + GetRandomScalar() * max_spawn_offset;

        DebugInterface* debug = agent_->Debug();
        debug->DebugMoveCamera(camera_pt_);
        debug->DebugCreateUnit(UNIT_TYPEID::TERRAN_MARINE, spawn_pt_, agent_->Observation()->GetPlayerID(), 1);
        debug->SendDebug();

        wait_game_loops_ = 5;
    }

    void OnStep() override {

    }

    void OnTestFinish() override {
        // Cleanup Test
        KillAllUnits();

        // Validate Result
        FeatureLayer8BPP data;
        const char* error = GetPlayerRelativeLayer(agent_, FeatureLayerType::MAP, data);
        if (error) {
            ReportError(error);
            return;
        }

        const GameInfo& game_info = agent_->Observation()->GetGameInfo();
        Point2D camera_world = agent_->Observation()->GetCameraPos();
        Point2DI camera = ConvertWorldToCamera(game_info, camera_world, spawn_pt_);

        if (!data.InBounds(camera)) {
            ReportError("Unit not in camera bounds");
            return;
        }

        if (data.Read(camera) == 0)
            ReportError("Unit not found in feature layer at expected location");
    }
};

//
// TestCameraMove
//

class TestCameraMove : public TestSequence {
public:
    Point2DI camera_pos_;

    void OnTestStart() override {
        ActionFeatureLayerInterface* action = agent_->ActionsFeatureLayer();
       
        const GameInfo& game_info = agent_->Observation()->GetGameInfo();

        const float kRandomOffset = 10.0f;

        Point2D test_point = FindCenterOfMap(game_info);
        test_point.x += GetRandomScalar() * kRandomOffset;
        test_point.y += GetRandomScalar() * kRandomOffset;

        camera_pos_ = ConvertWorldToMinimap(game_info, test_point);
        action->CameraMove(camera_pos_);

        wait_game_loops_ = 5;
    }

    void OnStep() override {

    }

    void OnTestFinish() override {
        const GameInfo& game_info = agent_->Observation()->GetGameInfo();
        Point2D camera_world = agent_->Observation()->GetCameraPos();
        Point2DI camera = ConvertWorldToMinimap(game_info, camera_world);

        if (camera.x != camera_pos_.x || camera.y != camera_pos_.y) {
            ReportError("Camera not at expected location");
        }
    }
};

class UnitCommandsFeatureLayer : public TestSequence {
public:
    Point2DI camera_pos_;
    Point2DI offset_pos_;
    Point2D unit_pos_;
    Point2DI unit_pos_i_;
    uint32_t starting_gameloop_;

    void OnTestStart() override {
        starting_gameloop_ = agent_->Observation()->GetGameLoop();
        const GameInfo& game_info = agent_->Observation()->GetGameInfo();
        unit_pos_ = FindCenterOfMap(game_info);
        agent_->Debug()->DebugCreateUnit(UNIT_TYPEID::ZERG_LURKERMP, unit_pos_);
        agent_->Debug()->DebugCreateUnit(UNIT_TYPEID::ZERG_LURKERMP, unit_pos_ + Point2D(5,5));
        agent_->Debug()->SendDebug();

        wait_game_loops_ = 550;
    }

    void OnStep() override {
        const ObservationInterface* obs = agent_->Observation();
        ActionFeatureLayerInterface* action = agent_->ActionsFeatureLayer();
        if (obs->GetGameLoop() == starting_gameloop_ + 20) {
            camera_pos_ = ConvertWorldToMinimap(obs->GetGameInfo(), unit_pos_);
            action->CameraMove(camera_pos_);
        }
        if (obs->GetGameLoop() == starting_gameloop_ + 21) {
            if (obs->GetFeatureLayerActions().camera_moves.empty()) {
                ReportError("Camera move is not being reported.");
            }
            else {
                Point2DI new_camera_pos = obs->GetFeatureLayerActions().camera_moves.front().center_minimap;
                if (new_camera_pos != camera_pos_) {
                    ReportError("Camera Move is reporting the incorrect location");
                }
            }
        }
        if (obs->GetGameLoop() == starting_gameloop_ + 24) {
            action->Select(Point2DI(0, 0), Point2DI(60, 60));
        }
        if (obs->GetGameLoop() == starting_gameloop_ + 25) {
            unit_pos_i_ = ConvertWorldToCamera(obs->GetGameInfo(), obs->GetCameraPos(), unit_pos_);
            action->Select(unit_pos_i_, PointSelectionType::PtToggle);
            if (obs->GetFeatureLayerActions().select_rects.empty()) {
                ReportError("Rectangle select is not being reported.");
            }
            else {
                if (obs->GetFeatureLayerActions().select_rects.front().select_screen.front().from != Point2DI(0, 0)) {
                    ReportError("Rectangle select is reporting the wrong start location");
                }
                if (obs->GetFeatureLayerActions().select_rects.front().select_screen.front().to != Point2DI(60, 60)) {
                    ReportError("Rectangle select is reporting the wrong end location");
                }
            }
        }
        if (obs->GetGameLoop() == starting_gameloop_ + 26) {
            action->UnitCommand(ABILITY_ID::MOVE, camera_pos_);
            if (obs->GetFeatureLayerActions().select_points.empty()) {
                ReportError("Select Point is not being reported.");
            }
            else {
                Point2DI select_pos = obs->GetFeatureLayerActions().select_points.front().select_screen;
                if (select_pos == unit_pos_i_) {
                    ReportError("Select Point is reporting the incorrect location");
                }
                if (obs->GetFeatureLayerActions().select_points.front().type != PointSelectionType::PtToggle) {
                    ReportError("Select Point is reporting the incorrect Selection Type");
                }
            }
        }
        if (obs->GetGameLoop() == starting_gameloop_ + 27) {
            action->UnitCommand(ABILITY_ID::BURROWDOWN);
            if (obs->GetFeatureLayerActions().unit_commands.empty()) {
                ReportError("Unit Command is not being reported.");
            }
            else {
                Point2DI target = obs->GetFeatureLayerActions().unit_commands.front().target;
                if (obs->GetFeatureLayerActions().unit_commands.front().ability_id != ABILITY_ID::MOVE) {
                    ReportError("Unit Command is reporting an incorrect Ability ID");
                }
                if (target != camera_pos_) {
                    ReportError("Unit Command is reporting an incorrect Target_location");
                }
            }
        }
        if (obs->GetGameLoop() == starting_gameloop_ + 28) {
            if (obs->GetFeatureLayerActions().unit_commands.empty()) {
                ReportError("Unit Command is not being reported.");
            }
            else if (obs->GetFeatureLayerActions().unit_commands.front().ability_id != ABILITY_ID::BURROWDOWN) {
                ReportError("Unit Command is reporting an incorrect Ability ID");
            }
        }
    }

    void OnTestFinish() override {
        KillAllUnits();
    }
};

//
// FeatureLayerTestBot
//

class FeatureLayerTestBot : public UnitTestBot {
public:
    FeatureLayerTestBot();

private:
    void OnTestsBegin () final;
    void OnTestsEnd () final;
};

FeatureLayerTestBot::FeatureLayerTestBot() :
    UnitTestBot() {
    // Sequences.
    for (int i = 0; i < 10; ++i)
        Add(TestCoordinateSystemMinimap());
    for (int i = 0; i < 10; ++i)
        Add(TestCoordinateSystemMap());
    for (int i = 0; i < 10; ++i)
        Add(TestCameraMove());

    Add(UnitCommandsFeatureLayer());
}

void FeatureLayerTestBot::OnTestsBegin() {
    Debug()->DebugShowMap();
};

void FeatureLayerTestBot::OnTestsEnd () {

}


//
// TestFeatureLayers
//

bool TestFeatureLayers(int argc, char** argv, const std::string& map, const sc2::FeatureLayerSettings& settings) {
    Coordinator coordinator;
    if (!coordinator.LoadSettings(argc, argv)) {
        return false;
    }

    coordinator.SetFeatureLayers(settings);

    // Add the custom bot, it will control the players.
    FeatureLayerTestBot bot;

    coordinator.SetParticipants({
        CreateParticipant(sc2::Race::Terran, &bot),
    });

    // Start the game.
    coordinator.LaunchStarcraft();
    coordinator.StartGame(map);

    // Step forward the game simulation.
    while (!bot.IsFinished()) {
        coordinator.Update();
    }

    return bot.Success();
}

bool TestFeatureLayers(int argc, char** argv) {
    bool success = true;

    sc2::FeatureLayerSettings sizeSquare(24.0f, 80, 80, 80, 80);
    sc2::FeatureLayerSettings sizeLong(24.0f, 80, 60, 80, 60);
    sc2::FeatureLayerSettings sizeTall(24.0f, 60, 80, 60, 80);

    success = TestFeatureLayers(argc, argv, sc2::kMapEmpty, sizeSquare) && success;
    success = TestFeatureLayers(argc, argv, sc2::kMapEmpty, sizeLong) && success;
    success = TestFeatureLayers(argc, argv, sc2::kMapEmpty, sizeTall) && success;

    success = TestFeatureLayers(argc, argv, sc2::kMapEmptyLong, sizeSquare) && success;
    success = TestFeatureLayers(argc, argv, sc2::kMapEmptyLong, sizeLong) && success;
    success = TestFeatureLayers(argc, argv, sc2::kMapEmptyLong, sizeTall) && success;

    success = TestFeatureLayers(argc, argv, sc2::kMapEmptyTall, sizeSquare) && success;
    success = TestFeatureLayers(argc, argv, sc2::kMapEmptyTall, sizeLong) && success;
    success = TestFeatureLayers(argc, argv, sc2::kMapEmptyTall, sizeTall) && success;

    return success;
}

}

