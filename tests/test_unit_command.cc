#include "test_unit_command_common.h"
#include "sc2api/sc2_api.h"
#include "sc2lib/sc2_lib.h"
#include <iostream>
#include <string>
#include <random>

namespace sc2 {
    //
    // TestAttackAttack
    //

    class TestAttackAttack : public TestUnitCommandTargetingPoint {
    public:
        TestAttackAttack() {
            test_unit_type_ = UNIT_TYPEID::TERRAN_MARINE;
            test_ability_ = ABILITY_ID::ATTACK;
        }

        void SetTestTime() override {
            wait_game_loops_ = 50;
        }
    };


    //
    // TestBehaviorCloakBanshee
    //

    class TestBehaviorCloakBanshee : public TestUnitCommandNoTarget {
    public:
        TestBehaviorCloakBanshee() {
            test_unit_type_ = UNIT_TYPEID::TERRAN_BANSHEE;
            test_ability_ = ABILITY_ID::BEHAVIOR_CLOAKON;
        }

        void OnTestFinish() override {
            Unit test_unit;
            if (GetTestUnit(test_unit)) {
                if (test_unit.cloak != Unit::CloakState::Cloaked) {
                    ReportError("Unit is not cloaked as expected.");
                }
            }
            else {
                ReportError("Could not find the test unit.");
            }

            VerifyUnitIdleAfterOrder(test_unit_type_);
            KillAllUnits();
        }

        void SetTestTime() override {
            wait_game_loops_ = 50;
        }
    };


    //
    // TestBehaviorSalvage
    //

    class TestBehaviorSalvage : public TestUnitCommandNoTarget {
    public:
        TestBehaviorSalvage() {
            test_unit_type_ = UNIT_TYPEID::TERRAN_BUNKER;
            test_ability_ = ABILITY_ID::EFFECT_SALVAGE;
        }

        void OnTestFinish() override {
            VerifyUnitDoesNotExist(test_unit_type_);
            KillAllUnits();
        }

        void SetTestTime() override {
            wait_game_loops_ = 200;
        }
    };


    //
    // TestBuildBarracks
    //

    class TestBuildBarracks : public TestUnitCommandTargetingPoint {
    public:
        TestBuildBarracks() {
            test_unit_type_ = UNIT_TYPEID::TERRAN_SCV;
            test_ability_ = ABILITY_ID::BUILD_BARRACKS;
            placing_structure_ = true;
        }

        void SetTestTime() override {
            wait_game_loops_ = 150;
        }

        void OnTestFinish() override {
            VerifyUnitExistsAndComplete(UNIT_TYPEID::TERRAN_BARRACKS);
            VerifyUnitIdleAfterOrder(test_unit_type_);
            VerifyUnitIdleAfterOrder(UNIT_TYPEID::TERRAN_BARRACKS);
            KillAllUnits();
        }
    };


    //
    // TestBuildBarracksTechLab
    //

    class TestBuildBarracksTechLab : public TestUnitCommandNoTarget {
    public:
        TestBuildBarracksTechLab() {
            test_unit_type_ = UNIT_TYPEID::TERRAN_BARRACKS;
            test_ability_ = ABILITY_ID::BUILD_TECHLAB;
        }

        void SetTestTime() override {
            wait_game_loops_ = 200;
        }

        void OnTestFinish() override {
            const ObservationInterface* obs = agent_->Observation();

            Units barracks_units = obs->GetUnits(Unit::Self, IsUnit(UNIT_TYPEID::TERRAN_BARRACKS));
            if (barracks_units.size() != 1) {
                ReportError("Could not find landed barracks.");
            }

            // Find the tech lab.
            Units techlab_units = obs->GetUnits(Unit::Self, IsUnit(UNIT_TYPEID::TERRAN_BARRACKSTECHLAB));
            if (techlab_units.size() != 1) {
                ReportError("Could not find a tech lab.");
            }

            if (barracks_units.size() == 1 && techlab_units.size() == 1) {
                if (barracks_units[0].add_on_tag == 0LL) {
                    ReportError("Barracks does not have an add-on.");
                }
                if (barracks_units[0].add_on_tag != techlab_units[0].tag) {
                    ReportError("Barracks add on tag does not match tech lab tag.");
                }
            }

            VerifyUnitExistsAndComplete(UNIT_TYPEID::TERRAN_BARRACKSTECHLAB);
            VerifyUnitIdleAfterOrder(test_unit_type_);
            VerifyUnitIdleAfterOrder(UNIT_TYPEID::TERRAN_BARRACKSTECHLAB);
            KillAllUnits();
        }
    };

    class TestBuildFlyingBarracksTechLab : public TestUnitCommandTargetingPoint {
    public:
        TestBuildFlyingBarracksTechLab() {
            test_unit_type_ = UNIT_TYPEID::TERRAN_BARRACKSFLYING;
            test_ability_ = ABILITY_ID::BUILD_TECHLAB;
        }

        void SetTestTime() override {
            wait_game_loops_ = 200;
        }

        void OnTestFinish() override {
            const ObservationInterface* obs = agent_->Observation();

            // Barracks will have landed. Find it.
            Units barracks_units = obs->GetUnits(Unit::Self, IsUnit(UNIT_TYPEID::TERRAN_BARRACKS));
            if (barracks_units.size() != 1) {
                ReportError("Could not find landed barracks.");
            }

            // Find the tech lab.
            Units techlab_units = obs->GetUnits(Unit::Self, IsUnit(UNIT_TYPEID::TERRAN_BARRACKSTECHLAB));
            if (techlab_units.size() != 1) {
                ReportError("Could not find a tech lab.");
            }

            if (barracks_units.size() == 1 && techlab_units.size() == 1) {
                if (barracks_units[0].add_on_tag == 0LL) {
                    ReportError("Barracks does not have an add-on.");
                }
                if (barracks_units[0].add_on_tag != techlab_units[0].tag) {
                    ReportError("Barracks add on tag does not match tech lab tag.");
                }
            }

            VerifyUnitExistsAndComplete(UNIT_TYPEID::TERRAN_BARRACKSTECHLAB);
            VerifyUnitIdleAfterOrder(UNIT_TYPEID::TERRAN_BARRACKS);
            VerifyUnitIdleAfterOrder(UNIT_TYPEID::TERRAN_BARRACKSTECHLAB);
            KillAllUnits();
        }
    };

    class TestBuildBarracksTechLabDifferentPoint : public TestUnitCommandTargetingPoint {
    public:
        TestBuildBarracksTechLabDifferentPoint() {
            test_unit_type_ = UNIT_TYPEID::TERRAN_BARRACKS;
            test_ability_ = ABILITY_ID::BUILD_TECHLAB;
        }

        void SetTestTime() override {
            wait_game_loops_ = 200;
        }

        void OnTestFinish() override {
            const ObservationInterface* obs = agent_->Observation();
            Units units = obs->GetUnits();

            // Barracks will have landed. Find it.
            Units barracks_units = obs->GetUnits(Unit::Self, IsUnit(UNIT_TYPEID::TERRAN_BARRACKS));
            if (barracks_units.size() != 1) {
                ReportError("Could not find landed barracks.");
            }

            // Find the tech lab.
            Units techlab_units = obs->GetUnits(Unit::Self, IsUnit(UNIT_TYPEID::TERRAN_BARRACKSTECHLAB));
            if (techlab_units.size() != 1) {
                ReportError("Could not find a tech lab.");
            }

            if (barracks_units.size() == 1 && techlab_units.size() == 1) {
                if (barracks_units[0].add_on_tag == 0LL) {
                    ReportError("Barracks does not have an add-on.");
                }
                if (barracks_units[0].add_on_tag != techlab_units[0].tag) {
                    ReportError("Barracks add on tag does not match tech lab tag.");
                }
            }

            VerifyUnitExistsAndComplete(UNIT_TYPEID::TERRAN_BARRACKSTECHLAB);
            VerifyUnitIdleAfterOrder(UNIT_TYPEID::TERRAN_BARRACKS);
            VerifyUnitIdleAfterOrder(UNIT_TYPEID::TERRAN_BARRACKSTECHLAB);
            KillAllUnits();
        }
    };

    class TestBuildBarracksTechLabSamePoint : public TestBuildBarracksTechLabDifferentPoint {
        void AdditionalTestSetup() override {
            target_point_ = origin_pt_;
        }
    };

    //
    // TestBuildExtractor
    //
    class TestBuildExtractor : public TestUnitCommandTargetingUnit {
    public:
        TestBuildExtractor() {
            test_unit_type_ = UNIT_TYPEID::ZERG_DRONE;
            target_unit_type_ = UNIT_TYPEID::NEUTRAL_VESPENEGEYSER;
            test_ability_ = ABILITY_ID::BUILD_EXTRACTOR;
            placing_structure_ = true;
        }

        void SetTestTime() override {
            wait_game_loops_ = 200;
        }

        void OnTestFinish() override {
            VerifyUnitExistsAndComplete(UNIT_TYPEID::ZERG_EXTRACTOR);
            KillAllUnits();
        }
    };


    //
    // TestBuildForge
    //

    class TestBuildForge : public TestUnitCommandTargetingPoint {
    public:
        TestBuildForge() {
            test_unit_type_ = UNIT_TYPEID::PROTOSS_PROBE;
            test_ability_ = ABILITY_ID::BUILD_FORGE;
            placing_structure_ = true;
        }

        void SetTestTime() override {
            wait_game_loops_ = 150;
        }

        void AdditionalTestSetup() override {
            target_point_ = GetPointOffsetX(origin_pt_);

            agent_->Debug()->DebugCreateUnit(UNIT_TYPEID::PROTOSS_PYLON, origin_pt_, agent_->Observation()->GetPlayerID(), 1);
            agent_->Debug()->SendDebug();
        }

        void OnTestFinish() override {
            VerifyUnitExistsAndComplete(UNIT_TYPEID::PROTOSS_FORGE);
            VerifyUnitIdleAfterOrder(test_unit_type_);
            VerifyUnitIdleAfterOrder(UNIT_TYPEID::PROTOSS_FORGE);
            KillAllUnits();
        }
    };


    //
    // TestBuildGateway
    //

    class TestBuildGateway : public TestUnitCommandTargetingPoint {
    public:
        TestBuildGateway() {
            test_unit_type_ = UNIT_TYPEID::PROTOSS_PROBE;
            test_ability_ = ABILITY_ID::BUILD_GATEWAY;
            placing_structure_ = true;
        }

        void SetTestTime() override {
            wait_game_loops_ = 150;
        }

        void AdditionalTestSetup() override {
            target_point_ = GetPointOffsetX(origin_pt_);

            agent_->Debug()->DebugCreateUnit(UNIT_TYPEID::PROTOSS_PYLON, origin_pt_, agent_->Observation()->GetPlayerID(), 1);
            agent_->Debug()->SendDebug();
        }

        void OnTestFinish() override {
            VerifyUnitExistsAndComplete(UNIT_TYPEID::PROTOSS_GATEWAY);
            VerifyUnitIdleAfterOrder(test_unit_type_);
            VerifyUnitIdleAfterOrder(UNIT_TYPEID::PROTOSS_GATEWAY);
            const ObservationInterface* obs = agent_->Observation();
            if (obs->GetWarpGateCount() != 0) {
                ReportError("Gateway is being incorrectly identified as a Warp Gate.");
            }
            KillAllUnits();
        }
    };


    //
    // TestBuildPylon
    //

    class TestBuildPylon : public TestUnitCommandTargetingPoint {
    public:
        TestBuildPylon() {
            test_unit_type_ = UNIT_TYPEID::PROTOSS_PROBE;
            test_ability_ = ABILITY_ID::BUILD_PYLON;
            placing_structure_ = true;
        }

        void SetTestTime() override {
            wait_game_loops_ = 150;
        }

        void OnTestFinish() override {
            VerifyUnitExistsAndComplete(UNIT_TYPEID::PROTOSS_PYLON);
            VerifyUnitIdleAfterOrder(test_unit_type_);
            const ObservationInterface* obs = agent_->Observation();
            if (obs->GetPowerSources().size() != 1) {
                ReportError("Pylon is not being counted as a power source.");
            }
            KillAllUnits();
        }
    };


    //
    // TestBuildSpineCrawler
    //

    class TestBuildSpineCrawler : public TestUnitCommandTargetingPoint {
    public:
        TestBuildSpineCrawler() {
            test_unit_type_ = UNIT_TYPEID::ZERG_DRONE;
            test_ability_ = ABILITY_ID::BUILD_SPINECRAWLER;
            placing_structure_ = true;
        }

        void SetTestTime() override {
            wait_game_loops_ = 450;
        }

        void AdditionalTestSetup() override {
            order_on_game_loop_ += 300;
            target_point_ = GetPointOffsetX(origin_pt_, -8);

            agent_->Debug()->DebugCreateUnit(UNIT_TYPEID::ZERG_CREEPTUMORBURROWED, GetPointOffsetX(origin_pt_, -10), agent_->Observation()->GetPlayerID(), 1);
            agent_->Debug()->SendDebug();
        }

        void OnTestFinish() override {
            VerifyUnitExistsAndComplete(UNIT_TYPEID::ZERG_SPINECRAWLER);
            KillAllUnits();
        }
    };


    //
    // TestCancelBuildInProgressFactory
    //

    class TestCancelBuildInProgressFactory : public TestUnitCommandNoTarget {
    public:
        Unit test_factory_;
        bool factory_built_ = false;

        TestCancelBuildInProgressFactory() {
            test_unit_type_ = UNIT_TYPEID::TERRAN_SCV;
            test_ability_ = ABILITY_ID::CANCEL;
        }

        void SetTestTime() override {
            wait_game_loops_ = 100;
        }

        void OnStep() override {
            const ObservationInterface* obs = agent_->Observation();
            ActionInterface* act = agent_->Actions();

            if (obs->GetGameLoop() < order_on_game_loop_) {
                return;
            }

            const Units& units = obs->GetUnits(Unit::Alliance::Self);
            for (const Unit& unit : units) {
                if (unit.unit_type == test_unit_type_) {
                    test_unit_ = unit;
                }
            }

            if (!factory_built_) {
                act->UnitCommand(test_unit_, ABILITY_ID::BUILD_FACTORY, origin_pt_);
                factory_built_ = true;
            }

            for (const Unit& unit : units) {
                if (unit.unit_type == UNIT_TYPEID::TERRAN_FACTORY) {
                    test_factory_ = unit;
                }
            }

            if (obs->GetGameLoop() < order_on_game_loop_ + 10) {
                return;
            }

            if (!orders_verified_ && factory_built_) {
                VerifyUnitOrders(test_unit_, ABILITY_ID::BUILD_FACTORY);
            }

            if (!ability_command_sent_) {
                VerifyUnitExistsAndComplete(UNIT_TYPEID::TERRAN_FACTORY, false);
                act->UnitCommand(test_factory_, test_ability_);
                ability_command_sent_ = true;
            }

        }

        void OnTestFinish() override {
            VerifyUnitIdleAfterOrder(test_unit_type_);
            VerifyUnitDoesNotExist(UNIT_TYPEID::TERRAN_FACTORY);
            KillAllUnits();
        }
    };


    //
    // TestCancelTrainingQueue
    //

    class TestCancelTrainingQueue : public TestUnitCommandNoTarget {
    public:
        bool marine_trained_ = false;

        TestCancelTrainingQueue() {
            test_unit_type_ = UNIT_TYPEID::TERRAN_BARRACKS;
            test_ability_ = ABILITY_ID::CANCEL_LAST;
        }

        void SetTestTime() override {
            wait_game_loops_ = 100;
        }

        void OnStep() override {
            const ObservationInterface* obs = agent_->Observation();
            ActionInterface* act = agent_->Actions();

            if (obs->GetGameLoop() < order_on_game_loop_) {
                return;
            }

            const Units& units = obs->GetUnits(Unit::Alliance::Self);
            for (const Unit& unit : units) {
                if (unit.unit_type == test_unit_type_) {
                    test_unit_ = unit;
                }
            }

            if (!marine_trained_) {
                act->UnitCommand(test_unit_, ABILITY_ID::TRAIN_MARINE);
                marine_trained_ = true;
            }

            if (obs->GetGameLoop() < order_on_game_loop_ + 10) {
                return;
            }

            if (!orders_verified_ && marine_trained_) {
                VerifyUnitOrders(test_unit_, ABILITY_ID::TRAIN_MARINE);
            }

            IssueUnitCommand(act);
        }

        void OnTestFinish() override {
            VerifyUnitIdleAfterOrder(test_unit_type_);
            VerifyUnitDoesNotExist(UNIT_TYPEID::TERRAN_MARINE);
            KillAllUnits();
        }
    };


    //
    // TestEffectArchon
    //

    class TestEffectArchon : public TestUnitCommandNoTarget {
    public:
        TestEffectArchon() {
            test_unit_type_ = UNIT_TYPEID::PROTOSS_HIGHTEMPLAR;
            test_ability_ = ABILITY_ID::MORPH_ARCHON;
        }

        void AdditionalTestSetup() override {
            agent_->Debug()->DebugCreateUnit(test_unit_type_, origin_pt_, agent_->Observation()->GetPlayerID(), 1);
            agent_->Debug()->SendDebug();
        }

        void SetTestTime() override {
            wait_game_loops_ = 50;
        }

        void OnTestFinish() override {
            VerifyUnitExistsAndComplete(UNIT_TYPEID::PROTOSS_ARCHON);
            VerifyUnitIdleAfterOrder(UNIT_TYPEID::PROTOSS_ARCHON);
            VerifyUnitDoesNotExist(UNIT_TYPEID::PROTOSS_HIGHTEMPLAR);
            KillAllUnits();
        }
    };


    //
    // TestEffectBlink
    //

    class TestEffectBlink : public TestUnitCommandTargetingPoint {
    public:
        TestEffectBlink() {
            test_unit_type_ = UNIT_TYPEID::PROTOSS_STALKER;
            test_ability_ = ABILITY_ID::EFFECT_BLINK;
            instant_cast_ = true;
        }

        void SetTestTime() override {
            wait_game_loops_ = 50;
        }

        void OnTestFinish() override {
            Unit test_unit;
            if (GetTestUnit(test_unit)) {
                if (Point2D(test_unit.pos) != target_point_) {
                    ReportError("New stalker location is incorrect, blink did not fire.");
                }
            }
            else {
                ReportError("Could not find the test unit.");
            }

            KillAllUnits();
        }
    };


    //
    // TestEffectHeal
    //

    class TestEffectHeal : public TestUnitCommandTargetingUnit {
    public:
        bool target_unit_damaged_ = false;

        TestEffectHeal() {
            test_unit_type_ = UNIT_TYPEID::TERRAN_MEDIVAC;
            target_unit_type_ = UNIT_TYPEID::TERRAN_MARAUDER;
            test_ability_ = ABILITY_ID::EFFECT_HEAL;
        }

        void SetTestTime() override {
            wait_game_loops_ = 250;
        }

        void AdditionalTestSetup() override {
            agent_->Debug()->DebugCreateUnit(target_unit_type_, GetPointOffsetX(origin_pt_, 10), agent_->Observation()->GetPlayerID(), 1);
            agent_->Debug()->SendDebug();
        }

        void OnStep() override {
            const ObservationInterface* obs = agent_->Observation();
            ActionInterface* act = agent_->Actions();
            const Units& units = obs->GetUnits();

            for (const Unit& unit : units) {
                if (unit.unit_type == target_unit_type_) {
                    target_unit_ = unit;
                }
            }

            if (obs->GetGameLoop() < order_on_game_loop_) {
                return;
            }

            if (!target_unit_damaged_) {
                agent_->Debug()->DebugSetLife(50, target_unit_);
                agent_->Debug()->SendDebug();
                target_unit_damaged_ = true;
            }

            if (obs->GetGameLoop() < order_on_game_loop_ + 5) {
                return;
            }

            if (!ability_command_sent_ && target_unit_.health != 50) {
                ReportError("Pre-ability target unit health is not correct.");
            }

            test_units_ = obs->GetUnits(Unit::Self, [&](const Unit& unit) {
                return unit.unit_type == test_unit_type_;
            });

            if (test_units_.size() > 0) {
                test_unit_ = test_units_.front();
            }

            IssueUnitCommand(act);
        }

        void OnTestFinish() override {
            if (target_unit_.health != target_unit_.health_max) {
                ReportError("Post-ability target unit health is not correct.");
            }
            KillAllUnits();
        }
    };


    //
    // TestEffectStimpack
    //

    class TestEffectStimpack : public TestUnitCommandNoTarget {
    public:
        TestEffectStimpack() {
            test_unit_type_ = UNIT_TYPEID::TERRAN_MARINE;
            test_ability_ = ABILITY_ID::EFFECT_STIM;
        }

        void SetTestTime() override {
            wait_game_loops_ = 50;
        }

        void OnTestFinish() override {
            Unit test_unit;
            if (GetTestUnit(test_unit)) {
                if (test_unit.buffs.front() != BUFF_ID::STIMPACK) {
                    ReportError("Stimpack buff is not present.");
                }
                if (test_unit.health == test_unit.health_max) {
                    ReportError("Unit health at unexpected value, stimpack did not fire.");
                }
            }
            else {
                ReportError("Could not find the test unit.");
            }

            VerifyUnitIdleAfterOrder(test_unit_type_);
            KillAllUnits();
        }
    };


    //
    // TestEffectScan
    //

    class TestEffectScan : public TestUnitCommandTargetingPoint {
    public:
        Unit test_hatchery_;
        bool hatchery_spawned_ = false;
        bool verify_pre_scan_ = false;
        bool verify_post_scan_ = false;

        TestEffectScan() {
            test_unit_type_ = UNIT_TYPEID::TERRAN_ORBITALCOMMAND;
            test_ability_ = ABILITY_ID::EFFECT_SCAN;
            instant_cast_ = true;
        }

        void SetTestTime() override {
            wait_game_loops_ = 300;
        }

        void AdditionalTestSetup() override {
            target_point_ = GetPointOffsetX(origin_pt_, 50);

            agent_->Debug()->DebugShowMap();
            agent_->Debug()->SendDebug();
        }

        void OnStep() override {
            const ObservationInterface* obs = agent_->Observation();
            ActionInterface* act = agent_->Actions();
            if (!hatchery_spawned_) {
                agent_->Debug()->DebugCreateUnit(UNIT_TYPEID::ZERG_HATCHERY, target_point_, agent_->Observation()->GetPlayerID() + 1, 1);
                agent_->Debug()->SendDebug();
                hatchery_spawned_ = true;
            }

            if (obs->GetGameLoop() < order_on_game_loop_) {
                return;
            }

            Units units = obs->GetUnits();

            Unit test_unit;
            for (const Unit& unit : units) {
                if (unit.unit_type == test_unit_type_) {
                    test_unit_ = unit.tag;
                    test_unit = unit;
                }
            }

            for (const Unit& unit : units) {
                if (unit.unit_type == UNIT_TYPEID::ZERG_HATCHERY) {
                    test_hatchery_ = unit;
                }
            }

            if (!verify_pre_scan_) {
                if (units.size() != 1 || units.front().unit_type != UNIT_TYPEID::TERRAN_ORBITALCOMMAND) {
                    ReportError("Enemy structure is not initially hidden.");
                }
                if (test_unit.energy < 50.30 || test_unit.energy > 50.32) {
                    ReportError("Test pre-ability unit energy is not correct.");
                }
                verify_pre_scan_ = true;
            }

            if (obs->GetGameLoop() < order_on_game_loop_ + 10) {
                return;
            }

            test_units_ = obs->GetUnits(Unit::Self, [&](const Unit& unit) {
                return unit.unit_type == test_unit_type_;
            });

            if (test_units_.size() > 0) {
                test_unit_ = test_units_.front();
                test_unit = test_units_.front();
            }

            IssueUnitCommand(act);

            if (obs->GetGameLoop() < order_on_game_loop_ + 15) {
                return;
            }

            if (!verify_post_scan_) {
                if (test_unit.energy < 0.83 || test_unit.energy > 0.85) {
                    ReportError("Test post-ability unit energy is not correct.");
                }
                if (test_hatchery_.display_type != Unit::DisplayType::Visible) {
                    ReportError("Enemy structure is not visible.");
                }
                if (test_hatchery_.is_on_screen == true) {
                    ReportError("Enemy structure on screen is true.");
                }
                if (test_hatchery_.alliance != Unit::Alliance::Enemy) {
                    ReportError("Enemy alliance is incorrect.");
                }
                if (test_hatchery_.owner != 2) {
                    ReportError("Owner of unit is incorrect.");
                }
                verify_post_scan_ = true;
            }
        }

        void OnTestFinish() override {
            if (test_hatchery_.display_type != Unit::DisplayType::Snapshot) {
                ReportError("On finish display type for hatchery is incorrect.");
            }
            if (test_hatchery_.owner != 2) {
                ReportError("Owner of unit is incorrect.");
            }
            KillAllUnits();
            agent_->Debug()->DebugShowMap();
            agent_->Debug()->SendDebug();
        }
    };


    //
    // TestHarvestProbeHarvestMinerals
    //

    class TestHarvestProbeHarvestMinerals : public TestUnitCommandTargetingUnit {
    public:
        TestHarvestProbeHarvestMinerals() {
            test_unit_type_ = UNIT_TYPEID::PROTOSS_PROBE;
            target_unit_type_ = UNIT_TYPEID::NEUTRAL_MINERALFIELD;
            test_ability_ = ABILITY_ID::HARVEST_GATHER;
        }

        void AdditionalTestSetup() override {
            target_point_ = GetPointOffsetX(origin_pt_);
            agent_->Debug()->DebugCreateUnit(target_unit_type_, target_point_, agent_->Observation()->GetPlayerID(), 1);
            agent_->Debug()->DebugCreateUnit(UNIT_TYPEID::PROTOSS_NEXUS, GetPointOffsetX(origin_pt_, -20), agent_->Observation()->GetPlayerID(), 1);
            agent_->Debug()->SendDebug();
        }

        void SetTestTime() override {
            wait_game_loops_ = 150;
        }

        void OnTestFinish() override {
            const ObservationInterface* obs = agent_->Observation();
            Units test_nexus_units = obs->GetUnits(Unit::Self, [&](const Unit& unit) {
                return unit.unit_type == UNIT_TYPEID::PROTOSS_NEXUS;
            });

            if (target_unit_.mineral_contents < 1400) {
                ReportError("Mineral patch does not contain any minerals.");
            }

            Unit test_unit;
            if (GetTestUnit(test_unit)) {
                if (test_unit.orders.front().ability_id != ABILITY_ID::HARVEST_RETURN) {
                    ReportError("Unit does not have correct post-harvest ability in orders.");
                }
            }
            else {
                ReportError("Could not find the test unit.");
            }

            // This issue is specific to debug spawns, so this may not be fixed, and this test is not valid.
            //if (test_nexus_units.front().assigned_harvesters != 1) {
            //    ReportError("Harvester is not registering as assigned");
            //}
            //if (test_nexus_units.front().ideal_harvesters < 1) {
            //    ReportError("Ideal harvesters is not being set.");
            //}

            KillAllUnits();
        }
    };


    //
    // TestHarvestProbeHarvestVespene
    //

    class TestHarvestProbeHarvestVespene : public TestUnitCommandTargetingUnit {
    public:
        TestHarvestProbeHarvestVespene() {
            test_unit_type_ = UNIT_TYPEID::PROTOSS_PROBE;
            target_unit_type_ = UNIT_TYPEID::PROTOSS_ASSIMILATOR;
            test_ability_ = ABILITY_ID::HARVEST_GATHER;
        }

        void AdditionalTestSetup() override {
            target_point_ = GetPointOffsetX(origin_pt_);
            agent_->Debug()->DebugCreateUnit(target_unit_type_, target_point_, agent_->Observation()->GetPlayerID(), 1);
            agent_->Debug()->DebugCreateUnit(UNIT_TYPEID::PROTOSS_NEXUS, GetPointOffsetX(origin_pt_, -20), agent_->Observation()->GetPlayerID(), 1);
            agent_->Debug()->SendDebug();
        }

        void SetTestTime() override {
            wait_game_loops_ = 150;
        }

        void OnTestFinish() override {
            const ObservationInterface* obs = agent_->Observation();
            Units test_nexus_units = obs->GetUnits(Unit::Self, [&](const Unit& unit) {
                return unit.unit_type == UNIT_TYPEID::PROTOSS_NEXUS;
            });

            // Ensure the vespene geyser has gas.
            {
                Units geysers = obs->GetUnits(Unit::Alliance::Neutral, IsUnit(UNIT_TYPEID::NEUTRAL_VESPENEGEYSER));
                if (geysers.size() < 1) {
                    ReportError("Could not find any geysers.");
                }
                else {
                    if (geysers[0].vespene_contents < 100){
                        ReportError("Geyser patch does not contain any vespene.");
                    }
                }
            }

            Unit test_unit;
            if (GetTestUnit(test_unit)) {
                if (test_unit.orders.front().ability_id != ABILITY_ID::HARVEST_RETURN) {
                    ReportError("Unit does not have correct post-harvest ability in orders.");
                }
            }
            else {
                ReportError("Could not find the test unit.");
            }

            // This issue is specific to debug spawns, so this may not be fixed, and this test is not valid.
            //if (target_unit_.assigned_harvesters != 1) {
            //    ReportError("Harvester is not registering as assigned");
            //}
            //if (target_unit_.ideal_harvesters < 1) {
            //    ReportError("Ideal harvesters is not being set.");
            //}

            KillAllUnits();
        }
    };


    //
    // TestMorphBroodLord
    //

    class TestMorphBroodLord : public TestUnitCommandNoTarget {
    public:
        TestMorphBroodLord() {
            test_unit_type_ = UNIT_TYPEID::ZERG_CORRUPTOR;
            test_ability_ = ABILITY_ID::MORPH_BROODLORD;
        }

        void SetTestTime() override {
            wait_game_loops_ = 100;
        }

        void OnTestFinish() override {
            VerifyUnitExistsAndComplete(UNIT_TYPEID::ZERG_BROODLORD);
            VerifyUnitDoesNotExist(UNIT_TYPEID::ZERG_CORRUPTOR);
            KillAllUnits();
        }
    };


    //
    // TestMorphLair
    //

    class TestMorphLair : public TestUnitCommandNoTarget {
    public:
        TestMorphLair() {
            test_unit_type_ = UNIT_TYPEID::ZERG_HATCHERY;
            test_ability_ = ABILITY_ID::MORPH_LAIR;
        }

        void SetOriginPoint() override {
            const GameInfo& game_info = agent_->Observation()->GetGameInfo();
            origin_pt_ = GetPointOffsetX(FindCenterOfMap(game_info), -10);
        }

        void SetTestTime() override {
            wait_game_loops_ = 100;
        }

        void OnTestFinish() override {
            VerifyUnitExistsAndComplete(UNIT_TYPEID::ZERG_LAIR);
            VerifyUnitDoesNotExist(UNIT_TYPEID::ZERG_SPIRE);
            KillAllUnits();
        }
    };


    //
    // TestMorphSiegeTankSiegeMode
    //

    class TestMorphSiegeTankSiegeMode : public TestUnitCommandNoTarget {
    public:
        TestMorphSiegeTankSiegeMode() {
            test_unit_type_ = UNIT_TYPEID::TERRAN_SIEGETANK;
            test_ability_ = ABILITY_ID::MORPH_SIEGEMODE;
        }

        void SetTestTime() override {
            wait_game_loops_ = 100;
        }

        void OnTestFinish() override {
            VerifyUnitExistsAndComplete(UNIT_TYPEID::TERRAN_SIEGETANKSIEGED);
            VerifyUnitDoesNotExist(UNIT_TYPEID::TERRAN_SIEGETANK);
            KillAllUnits();
        }
    };


    //
    // TestMorphStarportLand
    //

    class TestMorphStarportLand : public TestUnitCommandTargetingPoint {
    public:
        TestMorphStarportLand() {
            test_unit_type_ = UNIT_TYPEID::TERRAN_STARPORTFLYING;
            test_ability_ = ABILITY_ID::LAND;
            placing_structure_ = true;
        }

        void SetTestTime() override {
            wait_game_loops_ = 200;
        }

        void OnTestFinish() override {
            VerifyUnitExistsAndComplete(UNIT_TYPEID::TERRAN_STARPORT);
            VerifyUnitDoesNotExist(UNIT_TYPEID::TERRAN_STARPORTFLYING);
            KillAllUnits();
        }
    };


    //
    // TestMorphStarportLiftOff
    //

    class TestMorphStarportLiftOff : public TestUnitCommandNoTarget {
    public:
        TestMorphStarportLiftOff() {
            test_unit_type_ = UNIT_TYPEID::TERRAN_STARPORT;
            test_ability_ = ABILITY_ID::LIFT;
        }

        void SetTestTime() override {
            wait_game_loops_ = 100;
        }

        void OnTestFinish() override {
            VerifyUnitExistsAndComplete(UNIT_TYPEID::TERRAN_STARPORTFLYING);
            VerifyUnitDoesNotExist(UNIT_TYPEID::TERRAN_STARPORT);
            KillAllUnits();
        }
    };


    //
    // TestMoprphWarpGate
    //

    class TestMoprphWarpGate : public TestUnitCommandNoTarget {
    public:
        TestMoprphWarpGate() {
            test_unit_type_ = UNIT_TYPEID::PROTOSS_GATEWAY;
            test_ability_ = ABILITY_ID::MORPH_WARPGATE;
        }

        void SetTestTime() override {
            wait_game_loops_ = 150;
        }

        void AdditionalTestSetup() override {
            agent_->Debug()->DebugCreateUnit(UNIT_TYPEID::PROTOSS_PYLON, origin_pt_, agent_->Observation()->GetPlayerID(), 1);
            agent_->Debug()->SendDebug();
        }

        void OnTestFinish() override {
            VerifyUnitExistsAndComplete(UNIT_TYPEID::PROTOSS_WARPGATE);
            VerifyUnitIdleAfterOrder(UNIT_TYPEID::PROTOSS_WARPGATE);
            const ObservationInterface* obs = agent_->Observation();
            if (obs->GetWarpGateCount() != 1) {
                ReportError("ObservationInterface is not correctly detecting a Warp Gate.");
            }
            KillAllUnits();
        }
    };


    //
    // TestEffectMoveMove
    //

    class TestEffectMoveMove : public TestUnitCommandTargetingPoint {
    public:
        TestEffectMoveMove() {
            test_unit_type_ = UNIT_TYPEID::ZERG_HYDRALISK;
            test_ability_ = ABILITY_ID::MOVE;
        }

        void SetTestTime() override {
            wait_game_loops_ = 100;
        }

        void AdditionalTestSetup() override {
            target_point_ = GetPointOffsetX(origin_pt_, -5);
        }

        void OnTestFinish() override {
            Unit test_unit;
            if (GetTestUnit(test_unit)) {
                if (Point2D(test_unit.pos) != target_point_) {
                    ReportError("Unit did not move to the target point.");
                }
                if (test_unit.facing < 3.09300422 || test_unit.facing > 3.19114877) {
                    ReportError("Unit facing out of expected range after move.");
                }
            }
            else {
                ReportError("Could not find the test unit.");
            }

            KillAllUnits();
        }
    };


    //
    // TestEffectMovePatrol
    //

    class TestEffectMovePatrol : public TestUnitCommandTargetingPoint {
    public:
        TestEffectMovePatrol() {
            test_unit_type_ = UNIT_TYPEID::ZERG_HYDRALISK;
            test_ability_ = ABILITY_ID::PATROL;
        }

        void SetTestTime() override {
            wait_game_loops_ = 200;
        }

        void OnTestFinish() override {
            Unit test_unit;
            if (GetTestUnit(test_unit)) {
                Point2D patrol_target = test_unit.orders.front().target_pos;
                if ((patrol_target != target_point_) && (patrol_target != origin_pt_)) {
                    ReportError("Unit no longer has target patrol point in orders.");
                }
            }
            else {
                ReportError("Could not find the test unit.");
            }

            KillAllUnits();
        }
    };


    //
    // TestRallyRally
    //

    class TestRallyRally : public TestUnitCommandTargetingPoint {
    public:
        bool marine_trained_ = false;

        TestRallyRally() {
            test_unit_type_ = UNIT_TYPEID::TERRAN_BARRACKS;
            test_ability_ = ABILITY_ID::RALLY_UNITS;
            instant_cast_ = true;
        }

        void SetTestTime() override {
            wait_game_loops_ = 100;
        }

        void OnStep() override {
            const ObservationInterface* obs = agent_->Observation();
            ActionInterface* act = agent_->Actions();

            if (obs->GetGameLoop() < order_on_game_loop_) {
                return;
            }

            const Units& units = obs->GetUnits(Unit::Alliance::Self);
            for (const Unit& unit : units) {
                if (unit.unit_type == test_unit_type_) {
                    test_unit_ = unit;
                }
            }

            if (obs->GetGameLoop() < order_on_game_loop_ + 10) {
                return;
            }

            IssueUnitCommand(act);

            if (obs->GetGameLoop() < order_on_game_loop_ + 20) {
                return;
            }

            if (!marine_trained_) {
                act->UnitCommand(test_unit_, ABILITY_ID::TRAIN_MARINE);
                marine_trained_ = true;
            }
        }

        void OnTestFinish() override {
            VerifyUnitExistsAndComplete(UNIT_TYPEID::TERRAN_MARINE);
            VerifyUnitIdleAfterOrder(UNIT_TYPEID::TERRAN_MARINE);

            const ObservationInterface* obs = agent_->Observation();
            Units test_marine_units = obs->GetUnits(Unit::Self, [&](const Unit& unit) {
                return unit.unit_type == UNIT_TYPEID::TERRAN_MARINE;
            });

            if (Point2D(test_marine_units.front().pos) != target_point_) {
                ReportError("Trained marine is not at rally point.");
            }

            KillAllUnits();
        }
    };


    //
    // TestResearchTerranInfantryArmorLevel2
    //

    class TestResearchTerranInfantryArmorLevel2 : public TestUnitCommandNoTarget {
    public:
        std::string test_upgrade_name_ = "TerranInfantryArmorsLevel2";

        TestResearchTerranInfantryArmorLevel2() {
            test_unit_type_ = UNIT_TYPEID::TERRAN_ENGINEERINGBAY;
            test_ability_ = ABILITY_ID::RESEARCH_TERRANINFANTRYARMOR;
        }

        void SetTestTime() override {
            wait_game_loops_ = 200;
        }

        void AdditionalTestSetup() override {
            VerifyUpgradeOwned(test_upgrade_name_, true);
        }

        void OnTestFinish() override {
            VerifyUpgradeOwned(test_upgrade_name_);

            KillAllUnits();
        }
    };


    //
    // TestSensorTower
    //

    class TestSensorTower : public TestUnitCommand {
    public:
        TestSensorTower() {
            test_unit_type_ = UNIT_TYPEID::TERRAN_SENSORTOWER;
        }

        void SetTestTime() override {
            wait_game_loops_ = 50;
        }

        void AdditionalTestSetup() override {
            agent_->Debug()->DebugCreateUnit(UNIT_TYPEID::ZERG_MUTALISK, GetPointOffsetX(origin_pt_, 25), agent_->Observation()->GetPlayerID() + 1, 1);
            agent_->Debug()->DebugShowMap();
            agent_->Debug()->SendDebug();
        }

        void OnStep() override {}

        void OnTestFinish() override {
            const ObservationInterface* obs = agent_->Observation();
            const Units& target_units = obs->GetUnits(Unit::Alliance::Enemy);

            if (target_units.front().is_blip != true) {
                ReportError("Target unit is not a blip.");
            }
            if (target_units.front().cloak != Unit::CloakState::Unknown) {
                ReportError("Target unit cloak state is incorrect.");
            }
            if (target_units.front().display_type != Unit::DisplayType::Hidden) {
                ReportError("Target unit is not hidden.");
            }
            if (target_units.front().owner != 0) {
                ReportError("Owner of unit is incorrect.");
            }

            KillAllUnits();
            agent_->Debug()->DebugShowMap();
            agent_->Debug()->SendDebug();
        }
    };


    //
    // TestShields
    //

    class TestShields : public TestUnitCommandNoTarget {
    public:
        bool test_unit_damaged_ = false;
        bool shields_verified_ = false;

        TestShields() {
            test_unit_type_ = UNIT_TYPEID::PROTOSS_ZEALOT;
        }

        void SetTestTime() override {
            wait_game_loops_ = 50;
        }

        void OnStep() override {
            const ObservationInterface* obs = agent_->Observation();

            test_units_ = obs->GetUnits(Unit::Self, [&](const Unit& unit) {
                return unit.unit_type == test_unit_type_;
            });

            Unit test_unit;
            if (test_units_.size() > 0) {
                test_unit_ = test_units_.front().tag;
                test_unit = test_units_.front();
            }

            if (obs->GetGameLoop() < order_on_game_loop_) {
                return;
            }

            if (!test_unit_damaged_) {
                if (test_unit.shield != 50) {
                    ReportError("Pre-damage test unit shield is not correct.");
                }
                agent_->Debug()->DebugSetShields(25, test_unit_);
                agent_->Debug()->SendDebug();
                test_unit_damaged_ = true;
            }

            if (obs->GetGameLoop() < order_on_game_loop_ + 2) {
                return;
            }

            if (!shields_verified_) {
                if (test_unit.shield < 25 || test_unit.shield > 25.2) {
                    ReportError("Post-damage test unit shield is not correct.");
                }
                shields_verified_ = true;
            }
        }

        void VerifyUnitIdleAfterOrder(UNIT_TYPEID) override {}
    };


    //
    // TestStopStop
    //

    class TestStopStop : public TestUnitCommandNoTarget {
    public:
        bool move_command_sent_ = false;
        Point2D starting_point_;
        Point2D target_point_;

        TestStopStop() {
            test_unit_type_ = UNIT_TYPEID::PROTOSS_ZEALOT;
            test_ability_ = ABILITY_ID::STOP;
        }

        void AdditionalTestSetup() override {
            target_point_ = GetPointOffsetX(origin_pt_, 10);
        }

        void SetTestTime() override {
            wait_game_loops_ = 100;
        }

        void OnStep() override {
            const ObservationInterface* obs = agent_->Observation();
            ActionInterface* act = agent_->Actions();

            if (obs->GetGameLoop() < order_on_game_loop_) {
                return;
            }

            test_units_ = obs->GetUnits(Unit::Self, [&](const Unit& unit) {
                return unit.unit_type == test_unit_type_;
            });

            Unit test_unit;
            if (test_units_.size() > 0) {
                test_unit_ = test_units_.front().tag;
                test_unit = test_units_.front();
            }

            if (!move_command_sent_) {
                starting_point_ = Point2D(test_unit.pos);
                act->UnitCommand(test_unit_, ABILITY_ID::MOVE, target_point_);
                move_command_sent_ = true;
            }

            if (obs->GetGameLoop() < order_on_game_loop_ + 10) {
                return;
            }

            IssueUnitCommand(act);
        }

        void OnTestFinish() override {
            Unit test_unit;
            if (GetTestUnit(test_unit)) {
                if (Point2D(test_unit.pos) == starting_point_) {
                    ReportError("Unit did not start moving as expected.");
                }
                if (Point2D(test_unit.pos) == target_point_) {
                    ReportError("Unit did not stop moving as expected.");
                }
                if (test_unit.orders.size() != 0) {
                     ReportError("Unit still has orders that were not stopped.");
                }
            }
            else {
                ReportError("Could not find the test unit.");
            }

            KillAllUnits();
        }
    };


    //
    // TestTrainWarpZealot
    //

    class TestTrainWarpZealot : public TestUnitCommandTargetingPoint {
    public:
        TestTrainWarpZealot() {
            test_unit_type_ = UNIT_TYPEID::PROTOSS_WARPGATE;
            test_ability_ = ABILITY_ID::TRAINWARP_ZEALOT;
            instant_cast_ = true;
        }

        void SetTestTime() override {
            wait_game_loops_ = 150;
        }

        void AdditionalTestSetup() override {
            target_point_ = GetPointOffsetX(origin_pt_, 10);
            agent_->Debug()->DebugCreateUnit(UNIT_TYPEID::PROTOSS_PYLON, origin_pt_, agent_->Observation()->GetPlayerID(), 1);
            agent_->Debug()->DebugCreateUnit(UNIT_TYPEID::PROTOSS_PYLON, GetPointOffsetX(origin_pt_), agent_->Observation()->GetPlayerID(), 1);
            agent_->Debug()->SendDebug();
        }

        void OnTestFinish() override {
            VerifyUnitExistsAndComplete(UNIT_TYPEID::PROTOSS_ZEALOT);
            VerifyUnitIdleAfterOrder(UNIT_TYPEID::PROTOSS_ZEALOT);
            VerifyUnitIdleAfterOrder(UNIT_TYPEID::PROTOSS_WARPGATE);
            const ObservationInterface* obs = agent_->Observation();

            Units test_warped_zealot = obs->GetUnits(Unit::Self, [&](const Unit& unit) {
                return unit.unit_type == UNIT_TYPEID::PROTOSS_ZEALOT;
            });

            if (Point2D(test_warped_zealot.front().pos) != target_point_) {
                ReportError("Unit did not warp to target point.");
            }
            KillAllUnits();
        }
    };


    //
    // TestTrainBainling
    //

    class TestTrainBainling : public TestUnitCommandNoTarget {
    public:
        TestTrainBainling() {
            test_unit_type_ = UNIT_TYPEID::ZERG_ZERGLING;
            test_ability_ = ABILITY_ID::TRAIN_BANELING;
        }

        void SetTestTime() override {
            wait_game_loops_ = 50;
        }

        void OnTestFinish() override {
            VerifyUnitExistsAndComplete(UNIT_TYPEID::ZERG_BANELING);
            VerifyUnitIdleAfterOrder(UNIT_TYPEID::ZERG_BANELING);
            VerifyUnitDoesNotExist(UNIT_TYPEID::ZERG_ZERGLING);
            KillAllUnits();
        }
    };


    //
    // TestTrainMarine
    //

    class TestTrainMarine : public TestUnitCommandNoTarget {
    public:
        TestTrainMarine() {
            test_unit_type_ = UNIT_TYPEID::TERRAN_BARRACKS;
            test_ability_ = ABILITY_ID::TRAIN_MARINE;
        }

        void SetTestTime() override {
            wait_game_loops_ = 150;
        }

        void OnTestFinish() override {
            VerifyUnitExistsAndComplete(UNIT_TYPEID::TERRAN_MARINE);
            VerifyUnitIdleAfterOrder(test_unit_type_);
            VerifyUnitIdleAfterOrder(UNIT_TYPEID::TERRAN_MARINE);
            KillAllUnits();
        }
    };


    //
    // TestTrainRoach
    //

    class TestTrainRoach : public TestUnitCommandNoTarget {
    public:
        TestTrainRoach() {
            test_unit_type_ = UNIT_TYPEID::ZERG_HATCHERY;
            test_ability_ = ABILITY_ID::TRAIN_ROACH;
        }

        void SetTestTime() override {
            wait_game_loops_ = 150;
        }

        void SetOriginPoint() override {
            const GameInfo& game_info = agent_->Observation()->GetGameInfo();
            origin_pt_ = GetPointOffsetX(FindCenterOfMap(game_info), -10);
        }

        void AdditionalTestSetup() override {
            order_on_game_loop_ += 30;
            test_unit_type_ = UNIT_TYPEID::ZERG_LARVA;
        }

        void IssueUnitCommand(ActionInterface* act) override {
            if (ability_command_sent_) {
                return;
            }

            act->UnitCommand(test_unit_, test_ability_);
            act->UnitCommand(test_unit_, test_ability_);
            act->UnitCommand(test_unit_, test_ability_);
            ability_command_sent_ = true;
        }

        void OnTestFinish() override {
            const ObservationInterface* obs = agent_->Observation();
            Units roaches = obs->GetUnits(Unit::Self, [&](const Unit& unit) {
                return unit.unit_type == test_unit_type_;
            });

            if (roaches.size() != 3) {
                ReportError("The number of units trained was not correct.");
            }

            VerifyUnitExistsAndComplete(UNIT_TYPEID::ZERG_ROACH);
            VerifyUnitIdleAfterOrder(UNIT_TYPEID::ZERG_ROACH);
            KillAllUnits();
        }
    };


    //
    // TestTransportBunkerLoad
    //

    class TestTransportBunkerLoad : public TestUnitCommandTargetingUnit {
    public:
        bool marauder_order_verified_ = false;

        TestTransportBunkerLoad() {
            test_unit_type_ = UNIT_TYPEID::TERRAN_BUNKER;
            target_unit_type_ = UNIT_TYPEID::TERRAN_MARAUDER;
            test_ability_ = ABILITY_ID::LOAD;
            instant_cast_ = true;
        }

        void SetTestTime() override {
            wait_game_loops_ = 150;
        }

        void OnStep() override {
            const ObservationInterface* obs = agent_->Observation();
            ActionInterface* act = agent_->Actions();

            if (obs->GetGameLoop() < order_on_game_loop_) {
                return;
            }

            test_units_ = obs->GetUnits(Unit::Self, [&](const Unit& unit) {
                return unit.unit_type == test_unit_type_;
            });

            if (test_units_.size() > 0) {
                test_unit_ = test_units_.front().tag;
            }

            IssueUnitCommand(act);

            if (obs->GetGameLoop() < order_on_game_loop_ + 5) {
                return;
            }

            if (!orders_verified_ && ability_command_sent_) {
                VerifyUnitOrders(target_unit_, ABILITY_ID::MOVE);
            }
        }

        void OnTestFinish() override {
            Unit test_unit;
            if (GetTestUnit(test_unit)) {
                if (test_unit.passengers.front().tag != target_unit_.tag) {
                    ReportError("Passenger tag does not match target unit tag.");
                }
                if (test_unit.passengers.size() != 1) {
                    ReportError("Unit count in bunker is not 1.");
                }
                if (test_unit.cargo_space_max != 6) {
                    ReportError("Bunker cargo space max is not correct.");
                }
                if (test_unit.cargo_space_taken != 2) {
                    ReportError("Bunker cargo space taken is not correct.");
                }
            }
            else {
                ReportError("Could not find the test unit.");
            }

            KillAllUnits();
        }
    };


    //
    // TestTransportBunkerUnloadAll
    //

    class TestTransportBunkerUnloadAll : public TestUnitCommandNoTarget {
    public:
        bool marauders_loaded_ = false;

        TestTransportBunkerUnloadAll() {
            test_unit_type_ = UNIT_TYPEID::TERRAN_BUNKER;
            target_unit_type_ = UNIT_TYPEID::TERRAN_MARAUDER;
            test_ability_ = ABILITY_ID::UNLOADALL;
            instant_cast_ = true;
        }

        void SetTestTime() override {
            wait_game_loops_ = 150;
        }

        void AdditionalTestSetup() override {
            agent_->Debug()->DebugCreateUnit(UNIT_TYPEID::TERRAN_MARAUDER, origin_pt_, agent_->Observation()->GetPlayerID(), 3);
            agent_->Debug()->SendDebug();
        }

        void OnStep() override {
            const ObservationInterface* obs = agent_->Observation();
            ActionInterface* act = agent_->Actions();

            if (obs->GetGameLoop() < order_on_game_loop_) {
                return;
            }

            test_units_ = obs->GetUnits(Unit::Self, [&](const Unit& unit) {
                return unit.unit_type == test_unit_type_;
            });

            Unit test_unit;
            if (test_units_.size() > 0) {
                test_unit_ = test_units_.front().tag;
                test_unit = test_units_.front();
            }

            Units test_marauder_units = obs->GetUnits(Unit::Self, [&](const Unit& unit) {
                return unit.unit_type == UNIT_TYPEID::TERRAN_MARAUDER;
            });

            if (!marauders_loaded_) {
                act->UnitCommand(GetTagListFromUnits(test_marauder_units), ABILITY_ID::SMART, test_unit_);
                marauders_loaded_ = true;
            }

            if (obs->GetGameLoop() < order_on_game_loop_ + 10) {
                return;
            }

            if (!ability_command_sent_ && test_unit.cargo_space_taken != 6) {
                ReportError("Units are not present in bunker as expected.");
            }

            IssueUnitCommand(act);
        }

        void OnTestFinish() override {
            Unit test_unit;
            if (GetTestUnit(test_unit)) {
                if (test_unit.passengers.size() != 0) {
                    ReportError("Unit count in bunker is not 0.");
                }
                if (test_unit.cargo_space_max != 6) {
                    ReportError("Bunker cargo space max is not correct.");
                }
                if (test_unit.cargo_space_taken != 0) {
                    ReportError("Bunker cargo space taken is not correct.");
                }
            }
            else {
                ReportError("Could not find the test unit.");
            }
            KillAllUnits();
        }
    };



//
// UnitCommandTestBot
//

class UnitCommandTestBot : public UnitTestBot {
public:
    UnitCommandTestBot();

private:
    void OnTestsBegin () final;
    void OnTestsEnd () final;
};

UnitCommandTestBot::UnitCommandTestBot() :
    UnitTestBot() {
    // Sequences.
    Add(TestAttackAttack());
    Add(TestBehaviorCloakBanshee());
    Add(TestBehaviorSalvage());
    Add(TestBuildBarracks());
    Add(TestBuildBarracksTechLab());
    Add(TestBuildFlyingBarracksTechLab());
    Add(TestBuildBarracksTechLabDifferentPoint());
    Add(TestBuildBarracksTechLabSamePoint());
    Add(TestBuildExtractor());
    Add(TestBuildForge());
    Add(TestBuildGateway());
    Add(TestBuildPylon());
    Add(TestBuildSpineCrawler());
    Add(TestCancelBuildInProgressFactory());
    Add(TestCancelTrainingQueue());
    Add(TestEffectArchon());
    Add(TestEffectBlink());
    Add(TestEffectHeal());
    Add(TestEffectScan());
    Add(TestEffectStimpack());
    Add(TestHarvestProbeHarvestMinerals());
    Add(TestHarvestProbeHarvestVespene());
    Add(TestMorphBroodLord());
    Add(TestMorphLair());
    Add(TestMorphSiegeTankSiegeMode());
    //Add(TestMorphStarportLand());
    Add(TestMorphStarportLiftOff());
    Add(TestMoprphWarpGate());
    Add(TestEffectMoveMove());
    Add(TestEffectMovePatrol());
    Add(TestRallyRally());
    Add(TestResearchTerranInfantryArmorLevel2());
    Add(TestSensorTower());
    Add(TestShields());
    Add(TestStopStop());
    Add(TestTrainWarpZealot());
    Add(TestTrainBainling());
    Add(TestTrainMarine());
    Add(TestTrainRoach());
    Add(TestTransportBunkerLoad());
    Add(TestTransportBunkerUnloadAll());
}

void UnitCommandTestBot::OnTestsBegin() {
    Point2D map_center;

    map_center = FindCenterOfMap(Observation()->GetGameInfo());

    Debug()->DebugEnemyControl();
    Debug()->DebugShowMap();
    Debug()->DebugMoveCamera(map_center);
    Debug()->DebugGiveAllResources();
    Debug()->DebugIgnoreFood();
    Debug()->DebugGiveAllTech();
    Debug()->DebugGiveAllUpgrades();
    Debug()->DebugFastBuild();
};

void UnitCommandTestBot::OnTestsEnd () {

}

//
// TestUnitCommand
//

bool TestUnitCommand(int argc, char** argv) {
    Coordinator coordinator;
    if (!coordinator.LoadSettings(argc, argv)) {
        return false;
    }

    // Add the custom bot, it will control the players.
    UnitCommandTestBot bot;

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

