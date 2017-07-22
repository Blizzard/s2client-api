#pragma once

namespace sc2 {

#include "sc2api/sc2_common.h"
#include <string>
#include <cassert>

enum class FeatureLayerType {
    MAP,
    MINIMAP
};

struct FeatureLayer8BPP {
    bool InBounds(const Point2DI& pos) {
        return pos.x >= 0 && pos.x < width && pos.y >= 0 && pos.y < height;
    }

    char Read(const Point2DI& pos) {
        assert(InBounds(pos));
        return data[pos.x + pos.y * width];
    }

    int width;
    int height;
    std::string data;
};


Point2DI ConvertWorldToCamera(const GameInfo& game_info, const Point2D camera_world, const Point2D& world);
Point2DI ConvertWorldToMinimap(const GameInfo& game_info, const Point2D& world);
const char* GetPlayerRelativeLayer(Agent* agent, FeatureLayerType type, FeatureLayer8BPP& layer);

}