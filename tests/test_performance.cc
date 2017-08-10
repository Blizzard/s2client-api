#include "test_performance.h"

#include <iostream>
#include <iomanip>
#include <ctime>
#include <chrono>

#include "sc2api/sc2_api.h"

#include "test_framework.h"
#include "bot_examples.h"

using namespace std::chrono;

namespace sc2 {

struct EndOfTestsStats {
    std::vector<int> unit_count_;
    std::vector<int> structure_count_;
    std::vector<double> avg_ping_;
    std::vector<double> avg_observation_;
    bool reset_;

    void Reset() {
        unit_count_.clear();
        structure_count_.clear();
        avg_ping_.clear();
        avg_observation_.clear();
        reset_ = true;
    }
};

EndOfTestsStats& GetStats() {
    static EndOfTestsStats end_of_test_stats;
    return end_of_test_stats;
}

duration<double> TimedPing(ControlInterface* control, int ping_count) {
    high_resolution_clock::time_point start = high_resolution_clock::now();
    for (int i = 0; i < ping_count; ++i) {
        control->Ping();
    }
    return duration_cast<duration<double>>(high_resolution_clock::now() - start);
}

duration<double> TimedObservation(ControlInterface* control, int observation_count) {
    high_resolution_clock::time_point start = high_resolution_clock::now();
    for (int i = 0; i < observation_count; ++i) {
        control->GetObservation();
    }
    return duration_cast<duration<double>>(high_resolution_clock::now() - start);
}

class PingBot : public TestSequence {
public:
    void OnStep() final;

private:
    void OnTestStart() final;
    void OnTestFinish() final;

    int step_count_ = 0;
    double sum_ping_time_ = 0;

    static const int PING_COUNT = 50;
};

class FeatureLayerBot : public TestSequence {
public:
    void OnStep() final;

private:
    void OnTestStart() final;
    void OnTestFinish() final;

    int step_count_ = 0;

    double sum_observation_time_ = 0;
};

void PingBot::OnStep() {
    duration<double> time = TimedPing(agent_->Control(), PING_COUNT);
    double ping_avg = (time.count() / PING_COUNT) * 1000;
    //if (ping_avg > 5) {
    //    ReportError("Ping after step took longer than 5 milliseconds.");
    //}

    sum_ping_time_ += ping_avg;
    ++step_count_;
}

void PingBot::OnTestStart() {
    wait_game_loops_ = 100;

    DebugInterface* debug = agent_->Debug();

    EndOfTestsStats& stats = GetStats();

    static const int MARINE_COUNT = 50;
    static const int NEXUS_COUNT = 1;

    if (!stats.reset_) {
        debug->DebugShowMap();
        debug->DebugCreateUnit(UNIT_TYPEID::TERRAN_MARINE, Point2D(), agent_->Observation()->GetPlayerID(), MARINE_COUNT);
        debug->DebugCreateUnit(UNIT_TYPEID::PROTOSS_NEXUS, Point2D(), agent_->Observation()->GetPlayerID(), NEXUS_COUNT);
        debug->SendDebug();
    }

    stats.reset_ = false;

    if (stats.unit_count_.empty()) {
        stats.unit_count_.push_back(0);
    }
    else {
        stats.unit_count_.push_back(stats.unit_count_.back() + MARINE_COUNT);
    }

    if (stats.structure_count_.empty()) {
        stats.structure_count_.push_back(0);
    }
    else {
        stats.structure_count_.push_back(stats.structure_count_.back() + NEXUS_COUNT);
    }
};

void PingBot::OnTestFinish() {
    double avg_ping = sum_ping_time_ / step_count_;

    //if (avg_ping > 5) {
        //ReportError("In-Game ping test took longer than 5 milliseconds.");
    //}

    GetStats().avg_ping_.push_back(avg_ping);
}

void FeatureLayerBot::OnStep() {
    duration<double> time = TimedObservation(agent_->Control(), 1);
    double obs_time = time.count() * 1000;
    //if (obs_time > 30) {
    //    ReportError("Observation with feature layers should take not take longer than 20 milliseconds.");
    //}

    sum_observation_time_ += obs_time;
    ++step_count_;
}

void FeatureLayerBot::OnTestStart() {
    wait_game_loops_ = 100;
}

void FeatureLayerBot::OnTestFinish() {
    double avg_obs = sum_observation_time_ / step_count_;

    //if (avg_obs > 30) {
        //ReportError("In-Game ping test took longer than 5 milliseconds.");
    //}

    GetStats().avg_observation_.push_back(avg_obs);
}

class PerformanceTests : public UnitTestBot {
public:
    PerformanceTests(int feature_layer_width, int feature_layer_height);

    void PreGamePing();

private:
    void OnTestsBegin() final;
    void OnTestsEnd() final;

    static const int PING_COUNT = 1000;
    duration<double> ping_time_;
    int feature_layer_width_;
    int feature_layer_height_;
};

PerformanceTests::PerformanceTests(int feature_layer_width, int feature_layer_height) :
    UnitTestBot(),
    feature_layer_width_(feature_layer_width),
    feature_layer_height_(feature_layer_height) {

    GetStats().Reset();

    static const int TEST_COUNT = 7;
    for (int i = 0; i < TEST_COUNT; ++i) {
        Add(PingBot());
        Add(FeatureLayerBot());
    }
}

void PerformanceTests::PreGamePing() {
    ping_time_ = TimedPing(Control(), PING_COUNT);
}

void PerformanceTests::OnTestsBegin() {

}

void PerformanceTests::OnTestsEnd() {
    EndOfTestsStats& stats = GetStats();

    std::cout << std::endl << std::endl;

    std::cout << feature_layer_width_ << "x" << feature_layer_height_ << " Feature Layers" << std::endl;
    std::cout << "-------------------------------------------------------" << std::endl;

    std::cout << "|" << std::setw(10) << std::left << "Marines" << std::right << "|" << std::setw(10) << std::left << "Buildings" << std::right << "|" << std::setw(10) << std::left << "Ping (ms)" << std::right << "|" << std::setw(20) << std::left << "Observation (ms)" << std::right << "|" << std::endl;
    for (size_t i = 0; i < stats.avg_observation_.size(); ++i) {
        std::cout << "|" << std::setw(10) << std::left << stats.unit_count_[i] << std::right << "|" << std::setw(10) << std::left << stats.structure_count_[i] << std::right << "|" << std::setw(10) << std::left << stats.avg_ping_[i] << std::right << "|" << std::setw(20) << std::left << stats.avg_observation_[i] << std::right << "|" << std::endl;
    }
    std::cout << "-------------------------------------------------------" << std::endl;

    std::cout << std::endl << std::endl;
}

void TestPerformance(int argc, char** argv, int feature_layer_width, int feature_layer_height) {

    sc2::Coordinator coordinator;
    if (!coordinator.LoadSettings(argc, argv)) {
        return;
    }

    PerformanceTests performance_tests(feature_layer_width, feature_layer_height);

    FeatureLayerSettings settings;
    settings.map_x = feature_layer_height;
    settings.map_y = feature_layer_width;
    settings.minimap_x = feature_layer_height;
    settings.minimap_x = feature_layer_width;
    coordinator.SetFeatureLayers(settings);

    coordinator.SetParticipants({
        CreateParticipant(Race::Protoss, &performance_tests),
        CreateComputer(Race::Zerg)
    });

    coordinator.LaunchStarcraft();

    performance_tests.PreGamePing();

    coordinator.StartGame(sc2::kMapEmpty);

    // Step forward the game simulation.
    while (!performance_tests.IsFinished()) {
        coordinator.Update();
    }
}

bool TestPerformance(int argc, char** argv) {
    TestPerformance(argc, argv, 32, 32);
    TestPerformance(argc, argv, 64, 64);
    TestPerformance(argc, argv, 128, 128);
    TestPerformance(argc, argv, 256, 256);
    return true;
}

}