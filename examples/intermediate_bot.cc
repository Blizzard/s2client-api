#include "sc2api/sc2_api.h"
#include "sc2utils/sc2_manage_process.h"

#include "bot_examples.h"

#include <iostream>


static inline sc2::PlayerSetup CreateObserver(sc2::Agent* agent) {
    return sc2::PlayerSetup(sc2::PlayerType::Observer, sc2::Race::Terran, agent);
}

class ObserverBot : public sc2::Agent {

};

class Replay : public sc2::ReplayObserver {
public:
    std::vector<uint32_t> count_units_built_;

    Replay() :
        sc2::ReplayObserver() {
    }

    void OnGameStart() final {
        const sc2::ObservationInterface* obs = Observation();
        assert(obs->GetUnitTypeData().size() > 0);
        count_units_built_.resize(obs->GetUnitTypeData().size());
        std::fill(count_units_built_.begin(), count_units_built_.end(), 0);
    }

    void OnUnitCreated(const sc2::Unit* unit) final {
        assert(uint32_t(unit->unit_type) < count_units_built_.size());
        ++count_units_built_[unit->unit_type];
    }

    void OnStep() final {
    }

    void OnGameEnd() final {
        std::cout << "Units created:" << std::endl;
        const sc2::ObservationInterface* obs = Observation();
        const sc2::UnitTypes& unit_types = obs->GetUnitTypeData();
        for (uint32_t i = 0; i < count_units_built_.size(); ++i) {
            if (count_units_built_[i] == 0) {
                continue;
            }

            std::cout << unit_types[i].name << ": " << std::to_string(count_units_built_[i]) << std::endl;
        }
        std::cout << "Finished" << std::endl;
    }
};

int main(int argc, char* argv[]) {
    sc2::Coordinator coordinator;
    if (!coordinator.LoadSettings(argc, argv)) {
        return 1;
    }

    std::cout << "Here" << std::endl;

    std::cerr << "There" << std::endl;

    coordinator.SetMultithreaded(true);
//    coordinator.SetRealtime(true);
    // Add the custom bot, it will control the players.
    sc2::ProtossMultiplayerBot bot1;
    sc2::ZergMultiplayerBot bot2;
    sc2::TerranMultiplayerBot bot3;

    ObserverBot observerBot;

    coordinator.SetParticipants({
        CreateParticipant(sc2::Race::Protoss, &bot1)
        , CreateParticipant(sc2::Race::Zerg, &bot2)
        , CreateObserver(&observerBot)

//         , CreateComputer(sc2::Race::Terran, sc2::Difficulty::Hard)
//        , CreateParticipant(sc2::Race::Terran, &bot3)
     });

    // Start the game.
    coordinator.LaunchStarcraft();
    bool do_break = false;

//    for (; !do_break;) {
        coordinator.StartGame(sc2::kMapAridWastes);
//        coordinator.StartGame(sc2::kMapBelShirVestigeLE);

        //bot1.air_build_ = !bot1.air_build_;
        bot1.air_build_ = false;
        bot2.mutalisk_build_ = false;

        while (coordinator.Update() && !do_break) {
            if (sc2::PollKeyPress()) {
                do_break = true;
            }
        }
//    }
    bot1.Control()->DumpProtoUsage();
    bot2.Control()->DumpProtoUsage();
    return 0;
}