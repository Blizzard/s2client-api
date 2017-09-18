#include "test_movement_combat.h"
#include "sc2api/sc2_api.h"
#include "sc2lib/sc2_lib.h"
#include <iostream>
#include <string>
#include <random>

namespace sc2 {

//
// Wait10000
//

class Wait10000 : public TestSequence {
public:

    void OnTestStart() override {
        wait_game_loops_ = 10000;
    }

    void OnStep() override {
    }

    void OnTestFinish() override {
        KillAllUnits();
    }
};


//
// TestMarinesVsMarines
//

class TestMarinesVsMarines : public TestSequence {
public:
    Point2D battle_pt_;
    uint32_t attack_on_game_loop_;

    void OnTestStart() override {
        wait_game_loops_ = 1000;
        attack_on_game_loop_ = agent_->Observation()->GetGameLoop() + 10;

        const GameInfo& game_info = agent_->Observation()->GetGameInfo();
        Point2D friendly_rally_pt = FindRandomLocation(game_info);
        Point2D enemy_rally_pt = FindRandomLocation(game_info);

        battle_pt_.x = (friendly_rally_pt.x + enemy_rally_pt.x) / 2.0f;
        battle_pt_.y = (friendly_rally_pt.y + enemy_rally_pt.y) / 2.0f;
        agent_->Debug()->DebugCreateUnit(UNIT_TYPEID::TERRAN_MARINE, friendly_rally_pt, agent_->Observation()->GetPlayerID(), 20);
        agent_->Debug()->DebugCreateUnit(UNIT_TYPEID::TERRAN_MARINE, enemy_rally_pt, agent_->Observation()->GetPlayerID() + 1, 20);
        agent_->Debug()->SendDebug();
    }

    void OnStep() override {
        const ObservationInterface* obs = agent_->Observation();
        ActionInterface* act = agent_->Actions();
        if (obs->GetGameLoop() != attack_on_game_loop_)
            return;

        const Units& units = obs->GetUnits();
        for (const auto& unit : units) {
            act->UnitCommand(unit, ABILITY_ID::SMART, battle_pt_);
        }
    }

    void OnTestFinish() override {
        std::cout << "Finished on game loop: " << std::to_string(agent_->Observation()->GetGameLoop()) << std::endl;
        // At least 1/3 of the units should have died in this time.
        //if (agent_->Observation()->GetUnits().size() > 15)
        //    ReportError("Not all marines engaged.");

        KillAllUnits();
    }
};


//
// MovmentAndCombatTestBot
//

class MovmentAndCombatTestBot : public UnitTestBot {
public:
    MovmentAndCombatTestBot();

private:
    void OnTestsBegin () final;
    void OnTestsEnd () final;
};

MovmentAndCombatTestBot::MovmentAndCombatTestBot() :
    UnitTestBot() {
    // Sequences.
    //Add(Wait10000());
    Add(TestMarinesVsMarines());
    //Add(TestMarinesVsMarines());
    //Add(TestMarinesVsMarines());
    //Add(TestMarinesVsMarines());
    //Add(TestMarinesVsMarines());
}

void MovmentAndCombatTestBot::OnTestsBegin() {
    Debug()->DebugShowMap();
    Debug()->DebugEnemyControl();
};

void MovmentAndCombatTestBot::OnTestsEnd () {

}

//
// TestMovementCombat
//

bool TestMovementCombat(int argc, char** argv) {
    Coordinator coordinator;
    if (!coordinator.LoadSettings(argc, argv)) {
        return false;
    }

    // Add the custom bot, it will control the players.
    MovmentAndCombatTestBot bot;

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

