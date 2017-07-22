#include <sc2api/sc2_api.h>

using namespace sc2;

class Bot : public Agent {

};

int main(int argc, char** argv) {
    Coordinator coordinator;

    Bot bot;

    coordinator.SetParticipants({
        CreateParticipant(Race::Zerg, &bot),
        CreateComputer(Race::Terran)
    });

    coordinator.LaunchStarcraft("C:\t1\SC2.3.8-AlphaStar\Bin\Versions\Base00000\SC2_df_x64.exe");
    coordinator.StartGame(GetMapPath(Map::DaybreakLE));

    while (!coordinator.AllGamesEnded()) {
        coordinator.Step();
    }
}