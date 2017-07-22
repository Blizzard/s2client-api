#include "sc2api/sc2_game_settings.h"

namespace sc2 {

ProcessSettings::ProcessSettings(bool in_realtime,
    int in_step_size,
    const std::string& in_process_path,
    const std::string& in_net_address,
    int in_timeout_ms,
    int in_port_start,
    bool in_multi_threaded) :
    realtime(in_realtime),
    step_size(in_step_size),
    process_path(in_process_path),
    net_address(in_net_address),
    timeout_ms(in_timeout_ms),
    port_start(in_port_start),
    multi_threaded(in_multi_threaded) {
}

InterfaceSettings::InterfaceSettings() :
    use_feature_layers(false),
    use_render(false) {
}

GameSettings::GameSettings() :
    map_name(""),
    player_setup(),
    ports() {
}

ReplaySettings::ReplaySettings() :
    replay_file(),
    replay_dir(),
    player_id(1) {
}

std::string GetMapPath(Map map) {
    switch (map) {
        // Path relative to maps directory.
        case Map::AbyssalReefLE: return "Ladder/2017Season1/(2)AbyssalReefLE.SC2Map";
        case Map::BelShirVestigeLE: return "Ladder/2017Season1/(2)Bel'ShirVestigeLE (Void).SC2Map";
        case Map::NewkirkPrecinctTE: return "Ladder/2017Season1/(2)NewkirkPrecinctTE (Void).SC2Map";
        case Map::PaladinoTerminalLE: return "Ladder/2017Season1/(2)PaladinoTerminalLE.SC2Map";
        case Map::ProximaStationLE: return "Ladder/2017Season1/(2)ProximaStationLE.SC2Map";
        case Map::CactusValleyLE: return "Ladder/2017Season1/(4)CactusValleyLE (Void).SC2Map";
        case Map::HonorgroundsLE: return "Ladder/2017Season1/(4)HonorgroundsLE.SC2Map";
        default: return "";
    }
}

}
