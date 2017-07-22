#pragma once

#include "sc2api/sc2_game_settings.h"

namespace sc2 {
    bool ParseFromFile(ProcessSettings& process_settings, GameSettings& game_settings, const std::string& file_name);
    bool ParseSettings(int argc, char * argv[], ProcessSettings& process_settings, GameSettings& game_settings);
}