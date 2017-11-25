#include "test_movement_combat.h"
#include "test_restart.h"
#include "test_feature_layer.h"
#include "test_feature_layer_mp.h"
#include "test_multiplayer.h"
#include "test_rendered.h"
#include "test_snapshots.h"
#include "test_app.h"
#include "test_unit_command.h"
#include "test_performance.h"
#include "test_observation_interface.h"
#include "test_actions.h"
#include "sc2utils/sc2_manage_process.h"

#include <iostream>
#include <string>

// Tests. Easier to extern than create a .h for a single function prototype.
namespace sc2 {
bool TestAbilityRemap(int argc, char** argv);
}


#define TEST(X)                                                     \
    std::cout << "Running test: " << #X << std::endl;               \
    if (X(argc, argv)) {                                            \
        std::cout << "Test: " << #X << " succeeded." << std::endl;  \
    }                                                               \
    else {                                                          \
        success = false;                                            \
        std::cerr << "Test: " << #X << " failed!" << std::endl;     \
    }

//*************************************************************************************************
int main(int argc, char* argv[]) {
    bool success = true;

    // Add tests here.
    TEST(sc2::TestAbilityRemap);
    TEST(sc2::TestSnapshots);
    TEST(sc2::TestMultiplayer);
    TEST(sc2::TestMovementCombat);
    TEST(sc2::TestFastRestartSinglePlayer);
    TEST(sc2::TestUnitCommand);
    TEST(sc2::TestPerformance);
    TEST(sc2::TestObservationInterface);
    //TEST(sc2::TestObservationActions);
    TEST(sc2::TestRendered);

    if (success)
        std::cout << "All tests succeeded!" << std::endl;
    else
        std::cerr << "Some tests failed!" << std::endl;

    if (sc2::IsInDebugger()) {
        // When debugging, this prevents the console from disappearing before it can be read.
        std::cout << "Hit any key to exit..." << std::endl;
        while (!sc2::PollKeyPress());
    }

    return success ? 0 : -1;
}
