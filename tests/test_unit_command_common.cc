#include "test_framework.h"
#include "test_unit_command_common.h"
#include "sc2api/sc2_api.h"
#include "sc2lib/sc2_lib.h"
#include <iostream>
#include <string>
#include <random>

namespace sc2 {

    //
    // TestUnitCommand
    //

    void TestUnitCommand::OnTestStart() {
        SetTestTime();
        order_on_game_loop_ = agent_->Observation()->GetGameLoop() + 10;
        SetOriginPoint();

        agent_->Debug()->DebugCreateUnit(test_unit_type_, origin_pt_, agent_->Observation()->GetPlayerID(), 1);
        agent_->Debug()->SendDebug();
        AdditionalTestSetup();
    }

    void TestUnitCommand::OnStep() {
        const ObservationInterface* obs = agent_->Observation();
        ActionInterface* act = agent_->Actions();

        if (obs->GetGameLoop() < order_on_game_loop_) {
            return;
        }

        test_units_ = obs->GetUnits(Unit::Self, [&](const Unit& unit) {
            return unit.unit_type == test_unit_type_;
        });

        if (test_units_.size() > 0) {
            test_unit_ = test_units_.front();
        }

        IssueUnitCommand(act);
    }

    void TestUnitCommand::OnTestFinish() {
        VerifyUnitIdleAfterOrder(test_unit_type_);
        KillAllUnits();
    }

    void TestUnitCommand::OnUnitIdle(const Unit& unit) {
        uint32_t current_game_loop = agent_->Observation()->GetGameLoop();
        if (current_game_loop >= order_on_game_loop_) {
            idled_units_.push_back(unit);
        }
    }

    void TestUnitCommand::AdditionalTestSetup() {}

    void TestUnitCommand::IssueUnitCommand(ActionInterface*) {}

    Point2D TestUnitCommand::GetPointOffsetX(Point2D starting_point, float offset) {
        Point2D offset_point = starting_point;
        offset_point.x = starting_point.x + offset;
        return offset_point;
    }

    std::vector<Tag> TestUnitCommand::GetTagListFromUnits(Units units) {
        std::vector<Tag> tags;
        for (const auto& unit : units) {
            tags.push_back(unit.tag);
        }

        return tags;
    }

    void TestUnitCommand::SetOriginPoint() {
        const GameInfo& game_info = agent_->Observation()->GetGameInfo();
        origin_pt_ = FindCenterOfMap(game_info);
    }

    void TestUnitCommand::SetTestTime() {
        wait_game_loops_ = 150;
    }

    void TestUnitCommand::VerifyUnitOrders(Tag unit_tag, ABILITY_ID test_ability) {
        bool order_found = false;

        const ObservationInterface* obs = agent_->Observation();
        if (!obs) {
            return;
        }

        const Unit* unit = obs->GetUnit(unit_tag);
        if (!unit) {
            return;
        }

        for (UnitOrder order : unit->orders) {
            if (order.ability_id == test_ability) {
                order_found = true;
            }
        }

        if (!order_found) {
            ReportError("Expected order not in unit orders");
        }

        orders_verified_ = true;
    }

    void TestUnitCommand::VerifyUnitExistsAndComplete(UNIT_TYPEID unit_type, bool verify_complete) {
        bool unit_exists = false;
        Unit test_unit;

        const ObservationInterface* obs = agent_->Observation();
        const Units& units = obs->GetUnits(Unit::Alliance::Self);
        for (const Unit& unit : units) {
            if (unit.unit_type == unit_type) {
                unit_exists = true;
                test_unit = unit;
            }
        }

        if (test_unit.build_progress != 1.0f && verify_complete) {
            ReportError("Unit building/training did not complete as expected.");
        }

        if (!unit_exists) {
            ReportError("Expected unit does not exist.");
        }
    }

    void TestUnitCommand::VerifyUnitDoesNotExist(UNIT_TYPEID unit_type) {
        const ObservationInterface* obs = agent_->Observation();
        const Units& units = obs->GetUnits(Unit::Alliance::Self);
        for (const Unit& unit : units) {
            if (unit.unit_type == unit_type) {
                ReportError("Unexpected unit exists when it should not.");
            }
        }
    }

    void TestUnitCommand::VerifyUnitIdleAfterOrder(UNIT_TYPEID unit_type) {
        bool unit_idled = false;
        for (const Unit& unit : idled_units_) {
            if (unit.unit_type == unit_type) {
                unit_idled = true;
            }
        }

        if (!unit_idled) {
            ReportError("Unit did not trigger OnUnitIdle as expected.");
        }
    }

    void TestUnitCommand::VerifyOwnerOfUnits(Units units, int expected_owner) {
        for (Unit unit : units) {
            if (unit.owner != expected_owner) {
                ReportError("Owner of unit does not match expected value.");
            }
        }
    }

    void TestUnitCommand::VerifyUpgradeOwned(std::string test_upgrade_name, bool verify_not_owned) {
        unsigned int test_upgrade_id;
        bool upgrade_data_found = false;
        bool upgrade_owned = false;
        const ObservationInterface* obs = agent_->Observation();
        Upgrades upgrade_data = obs->GetUpgradeData();
        std::vector<UpgradeID> owned_upgrades = obs->GetUpgrades();

        for (sc2::UpgradeData upgrade : upgrade_data) {
            if (upgrade.name == test_upgrade_name) {
                test_upgrade_id = upgrade.upgrade_id;
                upgrade_data_found = true;
            }
        }

        if (!upgrade_data_found) {
            ReportError("Upgrade in test not found in upgrade data.");
            return;
        }

        for (UpgradeID upgrade : owned_upgrades) {
            if (upgrade == test_upgrade_id) {
                upgrade_owned = true;
            }
        }

        if (!upgrade_owned && !verify_not_owned) {
            ReportError("Upgrade in test is not owned.");
        }

        if (upgrade_owned && verify_not_owned) {
            ReportError("Upgrade in test is already owned.");
        }
    }

    bool TestUnitCommand::GetTestUnit(Unit& test_unit) {
        const ObservationInterface* obs = agent_->Observation();
        if (!obs) {
            return false;
        }
        const Unit* unit = obs->GetUnit(test_unit_);
        if (!unit) {
            return false;
        }

        test_unit = *unit;
        return true;
    }


    //
    // TestUnitCommandNoTarget
    //

    void TestUnitCommandNoTarget::IssueUnitCommand(ActionInterface* act) {
        if (ability_command_sent_) {
            return;
        }

        VerifyOwnerOfUnits(test_units_);

        if (test_units_.size() > 1) {
            act->UnitCommand(GetTagListFromUnits(test_units_), test_ability_);
        }
        else {
            act->UnitCommand(test_unit_, test_ability_);
        }

        ability_command_sent_ = true;

    }


    //
    // TestUnitCommandTargetingPoint
    //

    void TestUnitCommandTargetingPoint::AdditionalTestSetup() {
        target_point_ = GetPointOffsetX(origin_pt_);
    }

    void TestUnitCommandTargetingPoint::IssueUnitCommand(ActionInterface* act) {
        if (!orders_verified_ && ability_command_sent_) {
            if (!instant_cast_) {
                VerifyUnitOrders(test_unit_, test_ability_);
            }
        }

        if (ability_command_sent_) {
            return;
        }

        VerifyOwnerOfUnits(test_units_);

        if (placing_structure_ && !agent_->Query()->Placement(test_ability_, target_point_)) {
            ReportError("Cannot place structure, location not valid.");
            return;
        }

        if (test_units_.size() > 1) {
            act->UnitCommand(GetTagListFromUnits(test_units_), test_ability_, target_point_);
        }
        else {
            act->UnitCommand(test_unit_, test_ability_, target_point_);
        }

        ability_command_sent_ = true;
    }


    //
    // TestUnitCommandTargetingUnit
    //
    void TestUnitCommandTargetingUnit::AdditionalTestSetup() {
        target_point_ = GetPointOffsetX(origin_pt_);
        agent_->Debug()->DebugCreateUnit(target_unit_type_, target_point_, agent_->Observation()->GetPlayerID(), 1);
        agent_->Debug()->SendDebug();
    }

    void TestUnitCommandTargetingUnit::IssueUnitCommand(ActionInterface* act) {
        if (!orders_verified_ && ability_command_sent_) {
            if (!instant_cast_) {
                VerifyUnitOrders(test_unit_, test_ability_);
            }
        }

        if (ability_command_sent_) {
            return;
        }

        const ObservationInterface* obs = agent_->Observation();
        const Units& units = obs->GetUnits();

        for (const Unit& unit : units) {
            if (unit.unit_type == target_unit_type_) {
                target_unit_ = unit;
            }
        }

        VerifyOwnerOfUnits(test_units_);

        if (placing_structure_ && !agent_->Query()->Placement(test_ability_, target_unit_.pos)) {
            ReportError("Cannot place structure, location not valid.");
            return;
        }

        if (test_units_.size() > 1) {
            act->UnitCommand(GetTagListFromUnits(test_units_), test_ability_, target_unit_);
        }
        else {
            act->UnitCommand(test_unit_, test_ability_, target_unit_);
        }

        ability_command_sent_ = true;
    }
}
