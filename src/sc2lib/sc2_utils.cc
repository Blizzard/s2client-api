#include "sc2api/sc2_api.h"
#include "sc2lib/sc2_utils.h"

namespace sc2 {

Point2D FindRandomLocation(const Point2D& min, const Point2D& max) {
    Point2D target_pos;
    float playable_w = max.x - min.x;
    float playable_h = max.y - min.y;
    target_pos.x = playable_w * GetRandomFraction() + min.x;
    target_pos.y = playable_h * GetRandomFraction() + min.y;
    return target_pos;
}

Point2D FindRandomLocation(const GameInfo& game_info) {
    return FindRandomLocation(game_info.playable_min, game_info.playable_max);
}

Point2D FindCenterOfMap(const GameInfo& game_info) {
    Point2D target_pos;
    target_pos.x = game_info.playable_max.x / 2.0f;
    target_pos.y = game_info.playable_max.y / 2.0f;
    return target_pos;
}

}