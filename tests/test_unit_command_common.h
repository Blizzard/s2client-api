#pragma once

#include "test_framework.h"
#include "sc2api/sc2_coordinator.h"
#include "sc2api/sc2_agent.h"
#include "sc2api/sc2_typeenums.h"
#include "sc2api/sc2_common.h"
#include "sc2api/sc2_unit.h"
#include "sc2api/sc2_interfaces.h"
#include "sc2api/sc2_control_interfaces.h"
#include "sc2api/sc2_map_info.h"
#include <iostream>
#include <string>
#include <random>


namespace sc2 {

    class TestUnitCommand : public TestSequence {
    public:
        UNIT_TYPEID test_unit_type_;
        UNIT_TYPEID target_unit_type_;
        ABILITY_ID test_ability_;
        Tag test_unit_;
        Units test_units_;
        Units idled_units_;
        Point2D origin_pt_;
        uint32_t order_on_game_loop_;
        bool ability_command_sent_ = false;
        bool orders_verified_ = false;
        bool placing_structure_ = false;
        bool instant_cast_ = false;

        void OnTestStart() override;
        void OnStep() override;
        void OnTestFinish() override;
        void OnUnitIdle(const Unit& unit) final;
        virtual void AdditionalTestSetup();
        virtual void IssueUnitCommand(ActionInterface* act);
        virtual Point2D GetPointOffsetX(Point2D starting_point, float offset = 5);
        virtual std::vector<Tag> GetTagListFromUnits(Units units);
        virtual void SetOriginPoint();
        virtual void SetTestTime();
        virtual void VerifyUnitOrders(Tag unit_tag, ABILITY_ID test_ability);
        virtual void VerifyUnitExistsAndComplete(UNIT_TYPEID unit_type, bool verify_complete = true);
        virtual void VerifyUnitDoesNotExist(UNIT_TYPEID unit_type);
        virtual void VerifyUnitIdleAfterOrder(UNIT_TYPEID unit_type);
        virtual void VerifyUpgradeOwned(std::string test_upgrade_name, bool verify_not_owned = false);
        virtual void VerifyOwnerOfUnits(Units units, int expected_owner = 1);

        bool GetTestUnit(Unit& test_unit);
    };


    class TestUnitCommandNoTarget : public TestUnitCommand {
    public:
        void IssueUnitCommand(ActionInterface* act) override;
    };


    class TestUnitCommandTargetingPoint : public TestUnitCommand {
    public:
        Point2D target_point_;

        void AdditionalTestSetup() override;
        void IssueUnitCommand(ActionInterface* act) override;
    };


    class TestUnitCommandTargetingUnit : public TestUnitCommand {
    public:
        Unit target_unit_;
        Point2D target_point_;

        void AdditionalTestSetup() override;
        void IssueUnitCommand(ActionInterface* act) override;
    };


}