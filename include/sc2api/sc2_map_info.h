/*! \file sc2_map_info.h
    \brief Information about a game map.
*/
#pragma once

#include "sc2_common.h"
#include "sc2_gametypes.h"
#include <string>
#include <vector>

namespace sc2 {

//! Data for a feature layer or rendered image.
struct ImageData {
    int width;
    int height;
    int bits_per_pixel;
    std::string data;

    ImageData();
};

//! Rendered data for a game frame.
struct RenderedFrame {
    ImageData map;
    ImageData minimap;
};

//! Setup structure for feature layers or rendered images.
struct SpatialSetup {
    //! For feature layers only, determines the world space size of the camera.
    float camera_width;
    //! Number of pixels in X of the main game view.
    int map_resolution_x;
    //! Number of pixels in Y of the main game view.
    int map_resolution_y;
    //! Number of pixels in X of the minimap.
    int minimap_resolution_x;
    //! Number of pixels in Y of the minimap.
    int minimap_resolution_y;
};

//! Determines what type of data will be returned in observations.
struct InterfaceOptions {
    //! Raw data; essentially a list of units.
    bool raw;
    //! Feature layer data.
    SpatialSetup feature_layer;
    //! Rendered image data.
    SpatialSetup render;
};

struct PlayerInfo {
    uint32_t player_id;
    PlayerType player_type;
    Race race_requested;
    Race race_actual;
    Difficulty difficulty;

    PlayerInfo(uint32_t player_id, PlayerType player_type, Race race_requested, Race race_actual, Difficulty difficulty) :
        player_id(player_id),
        player_type(player_type),
        race_requested(race_requested),
        race_actual(race_actual),
        difficulty(difficulty) {};
};

//! Initial data for a game and map.
struct GameInfo {
    //! World width of a map.
    int width;
    //! World height of a map.
    int height;
    //! Grid showing which cells are pathable by units.
    ImageData pathing_grid;
    //! Height map of terrain.
    ImageData terrain_height;
    //! Grid showing which cells can accept placement of structures.
    ImageData placement_grid;
    //! The minimum coordinates of playable space. Points less than this are not playable.
    Point2D playable_min;
    //! The maximum coordinates of playable space. Points greater than this are not playable.
    Point2D playable_max;
    //! Positions of possible enemy starting locations.
    std::vector<Point2D> enemy_start_locations;

    //! Types of data that will be in observations.
    //!< \sa InterfaceOptions
    InterfaceOptions options;

    std::vector<PlayerInfo> player_info;

    GameInfo();
};

}
