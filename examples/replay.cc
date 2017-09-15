#include "sc2api/sc2_api.h"

#include "sc2utils/sc2_manage_process.h"

#include <iostream>

//this can either be the path to an individual replay or a folder containing replays
const char* kReplayFolder = "E:/Replays/";

class Replay : public sc2::ReplayObserver {
public:
    std::vector<uint32_t> count_units_built_;

    Replay() :
        sc2::ReplayObserver() {
    }

    

    //Called once when the game is started (or restarted)
    void OnGameStart() final {
    }
    
    //Called when a unit is created by the player being observed
    void OnUnitCreated(const sc2::Unit& unit) final {

    }

    //This is called when stepping through the game, and will not be called if the real time flag is passed to the coordinator.
    //In a realtime game, call GetObservation() instead. 
    void OnStep() final {
    }

    //Called when the game ends.
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

    //Given metadata about a replay, determine if it meets the criteria for replaying.
    //Common criteria: high mrr games, certain maps, certain match ups
    void IgnoreReplay(const ReplayInfo &replay_info, uint32_t &player_id)
    {
        //filter based on number of players 
        if (replay_info.num_players != 2)
        {
            return true;
        }

        //check stats about each individual player in the game
        for (unsigned int i = 0; i < replay_info.num_players; i++)
        {
            //filter based on mrr
            if (replay_info.players[i].mmr < 3800)
            {
                return true;
            }

            //filter based on match up
            //This will cause only terran vs terran replays
            if (replay_info.players[i].race == sc2::Terran)
            {
                return false;
            }
            else
            {
                return true;
            }
        }

        //filter by map
        if (replay_info.map_name == sc2::kMapBelShirVestigeLE)
        {
            return true;
        }
        
        //if all the tests passed, observe the replay
        return false;
    }
};


int main(int argc, char* argv[]) {
    sc2::Coordinator coordinator;

    //Make sure to pass the appropriate arguemnts when running the program.
    if (!coordinator.LoadSettings(argc, argv)) {
        return 1;
    }

    //load in the replay file or folder or replays
    if (!coordinator.SetReplayPath(kReplayFolder)) {
        std::cout << "Unable to load replay(s)." << std::endl;
        return 1;
    }

    //Keep processing replays until there are none left to process
    while (coordinator.HasReplays())
    {
        Replay* replay_observer = new Replay();

        //if you want to play process multiple replays simultaneously, call coordinator.AddReplayObserver(replay_observer).
        //Be sure to use a unique instance of replay_observer when doing this.
        coordinator.AddReplayObserver(replay_observer);

        //while there is a game to update, update it
        while (coordinator.Update());
        while (!sc2::PollKeyPress());

        //clean up memory for the next iteration
        delete replay_observer;
    }
}