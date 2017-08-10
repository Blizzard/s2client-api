#include "test_unit_command_common.h"
#include "sc2api/sc2_api.h"
#include "sc2lib/sc2_lib.h"
#include "test_observation_interface.h"

#include <iostream>
#include <string>
#include <random>


namespace sc2 {

    class TestGetAbilityData : public TestSequence{
    void OnTestFinish() {
        const ObservationInterface* obs = agent_->Observation();
        AbilityData ability = obs->GetAbilityData().at(static_cast<int>(ABILITY_ID::EFFECT_BLINK_STALKER));
        if (ability.link_name != "Blink") {
            std::string errorString = "Wrong Ability is pulled from function EFFECT_Blink is " + ability.link_name;
            ReportError(errorString.c_str());
        }

        if (ability.ability_id != ABILITY_ID::EFFECT_BLINK_STALKER) {
            ReportError("Wrong AbilityID");
        }

        if (ability.hotkey != "B") {
            std::string errorStr = "Wrong Hotkey " + ability.hotkey;
            ReportError(errorStr.c_str());
        }

        if (ability.is_building) {
            ReportError("is_building is reporting an incorrect value Blink does not create a building");
        }

        if (ability.target != sc2::AbilityData::Target::Point) {
            ReportError("Wrong targetValue");
        }

        if (ability.allow_autocast) {
            ReportError("Blink is not autocast");
        }

        AbilityData building_ability = obs->GetAbilityData().at(static_cast<int>(ABILITY_ID::BUILD_NEXUS));

        if (!building_ability.is_building) {
            ReportError("is_building is reporting an incorrect value BUILD_NEXUS creates a building");
        }
        
        if (building_ability.footprint_radius != 2.5f) {
            ReportError("footprint_radius is incorrect, should be 2.5");
        }

        AbilityData Autocast_ability = obs->GetAbilityData().at(static_cast<int>(ABILITY_ID::EFFECT_REPAIR));
        if (!Autocast_ability.allow_autocast) {
            ReportError("Repair is autocast");
        }

        AbilityData instant_placement_ability = obs->GetAbilityData().at(static_cast<int>(ABILITY_ID::BUILD_TECHLAB));
        if (!instant_placement_ability.is_instant_placement) {
            ReportError("build techlab should be instant placement");
        }

        KillAllUnits();
    }
    };

    class TestGetFoodCount : public TestSequence{
    void OnTestStart() {
        wait_game_loops_ = 10;
        const GameInfo& game_info = agent_->Observation()->GetGameInfo();
        Point2D origin_pt_ = FindCenterOfMap(game_info);
        Point2D offset_ = Point2D(10.0f, 10.0f);
        agent_->Debug()->DebugCreateUnit(UNIT_TYPEID::PROTOSS_ZEALOT, origin_pt_, agent_->Observation()->GetPlayerID(), 10);
        agent_->Debug()->DebugCreateUnit(UNIT_TYPEID::PROTOSS_PROBE, origin_pt_, agent_->Observation()->GetPlayerID(), 10);
        agent_->Debug()->DebugCreateUnit(UNIT_TYPEID::PROTOSS_PYLON, origin_pt_ + offset_, agent_->Observation()->GetPlayerID(), 2);
        agent_->Debug()->DebugCreateUnit(UNIT_TYPEID::PROTOSS_WARPGATE, origin_pt_ + offset_, agent_->Observation()->GetPlayerID(), 1);
        agent_->Debug()->SendDebug();
    }

    void OnTestFinish() {
        const ObservationInterface* obs = agent_->Observation();

        if (obs->GetArmyCount() != 10) {
            ReportError("Army Count is Incorrect");
        }
        if (obs->GetFoodArmy() != 20) {
            ReportError("Army Food is Incorrect");
        }
        if (obs->GetFoodWorkers() != 10) {
            ReportError("Worker Supply is Incorrect");
        }
        if (obs->GetIdleWorkerCount() != 10) {
            ReportError("Idle Worker Count is Incorrect");
        }
        if (obs->GetFoodUsed() != 30) {
            ReportError("Supply Count is Incorrect");
        }
        if (obs->GetFoodCap() != 16) {
            std::string errorStr = "Supply cap is Incorrect the supply is reporting 16 instead of " + std::to_string(obs->GetFoodCap());
            ReportError(errorStr.c_str());
        }
        if (obs->GetWarpGateCount() != 1) {
            ReportError("Warp gate Count is Incorrect");
        }
        if (obs->GetPowerSources().empty()) {
            ReportError("Reporting no Power sources when, there are pylons available.");
        }
        KillAllUnits();
    }
    };

    class TestGetBuffData : public TestSequence {
        void OnTestFinish() {
            const Buffs buffs = agent_->Observation()->GetBuffData();
            BuffData stim = buffs.at(static_cast<uint32_t>(BUFF_ID::STIMPACK));
            if (stim.buff_id != static_cast<uint32_t>(BUFF_ID::STIMPACK)) {
                ReportError("Wrong Buff ID returned");
            }
            if (stim.name != "Stimpack") {
                std::string errorStr = "Wrong Buff Name : " + stim.name;
                ReportError(errorStr.c_str());
            }
        }
    };

    class TestGetResources : public TestSequence {
        void OnTestStart() {
            wait_game_loops_ = 10;
            agent_->Debug()->DebugGiveAllResources();
            agent_->Debug()->SendDebug();
        }
        void OnTestFinish() {
            const ObservationInterface* obs = agent_->Observation();
            if (obs->GetMinerals() != 5000) {
                ReportError("Incorrect Mineral Count");
            }
            if (obs->GetVespene() != 5000) {
                ReportError("Incorrect Vespene Count");
            }
        }
    };

    class TestGetPreviousUnit : public TestSequence {
    public:
        uint32_t starting_game_loop;
        void OnTestStart() {
            wait_game_loops_ = 10;
            starting_game_loop = agent_->Observation()->GetGameLoop();
            const GameInfo& game_info = agent_->Observation()->GetGameInfo();
            Point2D origin_pt_ = FindCenterOfMap(game_info);
            agent_->Debug()->DebugCreateUnit(UNIT_TYPEID::TERRAN_MARINE, origin_pt_, agent_->Observation()->GetPlayerID(), 1);
            agent_->Debug()->SendDebug();
           
        }

        void OnStep() {
            const ObservationInterface* obs = agent_->Observation();

            //Throttle actions
            if (obs->GetGameLoop() == starting_game_loop + 2) {
                Units units = obs->GetUnits(Unit::Self, IsUnit(UNIT_TYPEID::TERRAN_MARINE));
                if (units.empty()) {
                    ReportError("No Unit Made");
                }

                agent_->Actions()->UnitCommand(units.front().tag, ABILITY_ID::EFFECT_STIM);
            }

            if (obs->GetGameLoop() == starting_game_loop + 3) {
                Units units = obs->GetUnits(Unit::Self, IsUnit(UNIT_TYPEID::TERRAN_MARINE));
                if (units.empty()) {
                    ReportError("No Unit Made");
                }

                const Unit* previous_unit = obs->GetPreviousUnit(units.front().tag);
                if (previous_unit->health != units.front().health_max && units.front().health != units.front().health_max - 10) {
                    ReportError("Get Previous Unit did not return the correct unit");
                }
            }
        }
        void OnTestFinish() {
            KillAllUnits();
        }
    };

    class TestGetUnitQueries : public TestSequence {
    public:
        uint32_t starting_game_loop;
        void OnTestStart() {
            wait_game_loops_ = 10;
            starting_game_loop = agent_->Observation()->GetGameLoop();

        }

        void OnStep() {
            const ObservationInterface* obs = agent_->Observation();
            const GameInfo& game_info = agent_->Observation()->GetGameInfo();
            Point2D origin_pt_ = FindCenterOfMap(game_info);
            //Throttle actions
            if (obs->GetGameLoop() == starting_game_loop + 2) {
                agent_->Debug()->DebugCreateUnit(UNIT_TYPEID::PROTOSS_ZEALOT, origin_pt_, agent_->Observation()->GetPlayerID(), 1);
                agent_->Debug()->SendDebug();
            }
            //Debug Messages take a while to send
            if (obs->GetGameLoop() == starting_game_loop + 4) {
                Units units_added = obs->GetUnitsAdded();
                if (units_added.empty()) {
                    ReportError("No units Reported from GetUnitsAdded() ");
                }
            }

            if (obs->GetGameLoop() == starting_game_loop + 5) {
                KillAllUnits();
            }

            if (obs->GetGameLoop() == starting_game_loop + 7) {
                Units units_removed = obs->GetUnitsRemoved();
                if (units_removed.empty()) {
                    ReportError("No units returned from getUnitsRemoved()");
                }
            }
        }

        void OnTestFinish() {
            KillAllUnits();
        }
    };

    class TestGetUnitData : public TestSequence {
        void OnTestStart() {
            const ObservationInterface* obs = agent_->Observation();
            const UnitTypes unit_data = obs->GetUnitTypeData();
            UnitTypeData stalker_data = unit_data.at(static_cast<uint32_t>(UNIT_TYPEID::PROTOSS_STALKER));
            UnitTypeData Overlord_data = unit_data.at(static_cast<uint32_t>(UNIT_TYPEID::ZERG_OVERLORDTRANSPORT));
            const GameInfo& game_info = agent_->Observation()->GetGameInfo();
            Point2D origin_pt_ = FindCenterOfMap(game_info);
            agent_->Debug()->DebugCreateUnit(UNIT_TYPEID::PROTOSS_STALKER, origin_pt_, agent_->Observation()->GetPlayerID(), 1);
            agent_->Debug()->SendDebug();

            if (stalker_data.attributes.empty()) {
                std::string errorStr = "UnitTypeData is reporting an no attributes";
                ReportError(errorStr.c_str());
            }
            if (!stalker_data.available) {
                std::string errorStr = "UnitTypeData is reporting stalkers are unavailable";
                ReportError(errorStr.c_str());
            }
            if (stalker_data.cargo_size != 2) {
                std::string errorStr = "UnitTypeData is reporting an incorrect cargo value. Expected : 0 Result : " + std::to_string(stalker_data.cargo_size);
                ReportError(errorStr.c_str());
            }
            if (stalker_data.mineral_cost != 125) {
                std::string errorStr = "UnitTypeData is reporting an incorrect mineral cost value. Expected : 125 Result : " + std::to_string(stalker_data.mineral_cost);
                ReportError(errorStr.c_str());
            }
            if (stalker_data.vespene_cost != 50) {
                std::string errorStr = "UnitTypeData is reporting an incorrect vespene value. Expected : 50 Result : " + std::to_string(stalker_data.vespene_cost);
                ReportError(errorStr.c_str());
            }
            if (stalker_data.name != "Stalker") {
                std::string errorStr = "UnitTypeData is reporting an incorrect name. Expected : Stalker Result : " + stalker_data.name;
                ReportError(errorStr.c_str());
            }
            if (stalker_data.unit_type_id != UNIT_TYPEID::PROTOSS_STALKER) {
                std::string errorStr = "UnitTypeData is reporting an incorrect unittypeID Expected : 74 Result : " + std::to_string(stalker_data.unit_type_id);
                ReportError(errorStr.c_str());
            }
            if (stalker_data.weapons.empty()) {
                std::string errorStr = "UnitTypeData is reporting no weapons";
                ReportError(errorStr.c_str());
            }
            if (!Overlord_data.weapons.empty()) {
                std::string errorStr = "UnitTypeData is reporting weapons for an overlord";
                ReportError(errorStr.c_str());
            }
        }
        
        void OnTestFinish() {
            KillAllUnits();
        }
    };
//
// UnitCommandTestBot
//

class TestObservationBot : public UnitTestBot {
public:
    TestObservationBot();

private:
    void OnTestsBegin () final;
    void OnTestsEnd () final;
};

TestObservationBot::TestObservationBot() :
    UnitTestBot() {
    // Sequences.
    Add(TestGetUnitData());
    Add(TestGetAbilityData());
    Add(TestGetFoodCount());
    Add(TestGetBuffData());
    Add(TestGetResources());
    Add(TestGetPreviousUnit());
    Add(TestGetUnitQueries());
}

void TestObservationBot::OnTestsBegin() {
    Debug()->DebugGiveAllUpgrades();
};

void TestObservationBot::OnTestsEnd () {

}

//
// TestUnitCommand
//

bool TestObservationInterface(int argc, char** argv) {
    Coordinator coordinator;
    if (!coordinator.LoadSettings(argc, argv)) {
        return false;
    }

    // Add the custom bot, it will control the players.
    TestObservationBot bot;

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

