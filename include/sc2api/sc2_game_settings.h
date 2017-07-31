#pragma once

#include "sc2api/sc2_gametypes.h"

#include <string>
#include <vector>

namespace sc2 {

//! Information about a running process.
struct ProcessInfo {
    std::string process_path;
    uint64_t process_id;
    int port;
};

//! Settings to run the game process.
struct ProcessSettings {
    ProcessSettings(bool in_real_time,
        int in_step_size,
        const std::string& in_process_path,
        const std::string& in_net_address,
        int in_timeout_ms,
        int in_port_start,
        bool in_multi_threaded = false);

    bool realtime;
    int step_size;
    std::string process_path;
    std::string net_address;
    int timeout_ms;
    int port_start;
    // Run all OnSteps in parallel.
    bool multi_threaded;
    std::vector<std::string> extra_command_lines;
    // PID and port of all running sc2 processes.
    std::vector<ProcessInfo> process_info;
};

//! Settings for an RGB rendered output.
struct RenderSettings {
    RenderSettings() = default;
    RenderSettings(int map_x, int map_y, int minimap_x, int minimap_y)
        : map_x(map_x), map_y(map_y), minimap_x(minimap_x), minimap_y(minimap_y)
    {}
    //! X size of rendered bitmap for the world screen.
    int map_x = 800;
    //! Y size of rendered bitmap for the world screen.
    int map_y = 600;
    //! X size of rendered bitmap for the minimap.
    int minimap_x = 300;
    //! Y size of rendered bitmap for the minimap.
    int minimap_y = 300;
};

//! Settings for feature layer output.
struct FeatureLayerSettings {
    FeatureLayerSettings () = default;
    FeatureLayerSettings (float in_camera_width, int in_map_x, int in_map_y, int in_minimap_x, int in_minimap_y)
        : camera_width(in_camera_width), map_x(in_map_x), map_y(in_map_y), minimap_x(in_minimap_x), minimap_y(in_minimap_y)
    {}
    //! The width of the camera view as it would appear in the world. The size is in game world units.
    float camera_width = 24.0f;
    //! X size of the feature layer bitmap for the world screen.
    int map_x = 64;
    //! Y size of the feature layer bitmap for the world screen.
    int map_y = 64;
    //! X size of the feature layer bitmap for the minimap.
    int minimap_x = 64;
    //! Y size of the feature layer bitmap for the minimap.
    int minimap_y  = 64;
};

//! Settings for rendered feature layer output.
struct InterfaceSettings {
    InterfaceSettings();

    bool use_feature_layers;
    FeatureLayerSettings feature_layer_settings;
    bool use_render;
    RenderSettings render_settings;
};

//! Settings for starting a game.
struct GameSettings {
    GameSettings();

    std::string map_name;
    std::vector<PlayerSetup> player_setup;
    Ports ports;
};

//! Settings for starting a replay.
struct ReplaySettings {
    ReplaySettings();

    // Fill with replays to analyze.
    std::vector<std::string> replay_file;
    std::string replay_dir;
    uint32_t player_id;
};

//! Game status.
enum class AppState {
    normal,         // The game application has behaved normally.
    timeout,        // A timeout has occurred, and the game application was terminated.
    timeout_zombie, // A timeout has occurred, but the game application could not be terminated.
    crashed         // A crash has been detected.
};

//! Known maps.
//! These are the maps that come with the repo.
//! See documentation for how to download and install additional map packs.
extern const char* kMapBelShirVestigeLE;
extern const char* kMapEmpty;
extern const char* kMapEmptyLong;
extern const char* kMapEmptyTall;
extern const char* kMapMarineMicro;

}
