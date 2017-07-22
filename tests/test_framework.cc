#include "test_framework.h"
#include "sc2api/sc2_api.h"

#include <iostream>
#include <string>
#include <random>

namespace sc2 {

//
// TestSequence
//

TestSequence::TestSequence() :
    agent_(nullptr),
    wait_game_loops_(5) {
}

bool TestSequence::DidSucceed() const {
    return errors_.empty();
}

void TestSequence::ReportError(const char* error) {
    test_name_ = typeid(*this).name();
    test_name_ = test_name_.substr(test_name_.find("::") + 2, -1);

    errors_.push_back(error);
}

void TestSequence::KillAllUnits() {
    const Units& units = agent_->Observation()->GetUnits();
    for (const Unit& unit : units) {
        agent_->Debug()->DebugKillUnit(unit.tag);
        for (auto passenger : unit.passengers) {
            agent_->Debug()->DebugKillUnit(passenger.tag);
        }
    }
    agent_->Debug()->SendDebug();
}


//
// TestSequence
//

void UnitTestBot::OnStep() {
    const ObservationInterface* obs = Observation();
    uint32_t game_loop = obs->GetGameLoop();

    // Move to the next sequence if this sequence is done.
    if (game_loop >= game_loop_done_) {
        if (current_sequence_ == std::size_t(-1)) {
            current_sequence_ = 0;
            OnTestsBegin();
        }
        else {
            sequences_[current_sequence_]->OnTestFinish();
            if (!sequences_[current_sequence_]->DidSucceed()) {
                success_ = false;
                std::cout << "Test: " << sequences_[current_sequence_]->test_name_ << " failed!" << std::endl;
                 for (const std::string& error : sequences_[current_sequence_]->errors_) {
                    std::cout << "    Error: " << error << std::endl;
                }
            }
            ++current_sequence_;
        }
        if (IsFinished()) {
            OnTestsEnd();
            return;
        }

        sequences_[current_sequence_]->OnTestStart();
        game_loop_done_ = game_loop + sequences_[current_sequence_]->wait_game_loops_;
        return;
    }
    if (current_sequence_ == std::size_t(-1))
        return;

    sequences_[current_sequence_]->OnStep();

    OnPostStep();
}

void UnitTestBot::OnGameStart() {
    if (current_sequence_ < sequences_.size()) {
        sequences_[current_sequence_]->OnGameStart();
    }
}

void UnitTestBot::OnGameEnd() {
    if (current_sequence_ < sequences_.size()) {
        sequences_[current_sequence_]->OnGameEnd();
    }

    if (!IsFinished()) {
        success_ = false;
        current_sequence_ = sequences_.size() + 1;
        std::cout << "Game ended before tests finished." << std::endl;
        return;
    }
}

void UnitTestBot::OnGameFullStart() {
    if (current_sequence_ < sequences_.size()) {
        sequences_[current_sequence_]->OnGameFullStart();
    }
}

void UnitTestBot::OnUnitDestroyed(const Unit& unit) {
    if (current_sequence_ < sequences_.size()) {
        sequences_[current_sequence_]->OnUnitDestroyed(unit);
    }
}

void UnitTestBot::OnUnitCreated(const Unit& unit) {
    if (current_sequence_ < sequences_.size()) {
        sequences_[current_sequence_]->OnUnitCreated(unit);
    }
}

void UnitTestBot::OnUnitIdle(const Unit& unit) {
    if (current_sequence_ < sequences_.size()) {
        sequences_[current_sequence_]->OnUnitIdle(unit);
    }
}

void UnitTestBot::OnUnitEnterVision(const Unit& unit) {
    if (current_sequence_ < sequences_.size()) {
        sequences_[current_sequence_]->OnUnitEnterVision(unit);
    }
}

void UnitTestBot::OnUpgradeCompleted(UpgradeID upgrade) {
    if (current_sequence_ < sequences_.size()) {
        sequences_[current_sequence_]->OnUpgradeCompleted(upgrade);
    }
}

void UnitTestBot::OnBuildingConstructionComplete(const Unit& unit) {
    if (current_sequence_ < sequences_.size()) {
        sequences_[current_sequence_]->OnBuildingConstructionComplete(unit);
    }
}

void UnitTestBot::OnNydusDetected() {
    if (current_sequence_ < sequences_.size()) {
        sequences_[current_sequence_]->OnNydusDetected();
    }
}

void UnitTestBot::OnNuclearLaunchDetected() {
    if (current_sequence_ < sequences_.size()) {
        sequences_[current_sequence_]->OnNuclearLaunchDetected();
    }
}

}

