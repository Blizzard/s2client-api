#pragma once

#include "sc2api/sc2_map_info.h"
#include "sc2api/sc2_common.h"

#include <iostream>
#include <string>
#include <random>

namespace sc2 {

Point2D FindRandomLocation(const Point2D& min, const Point2D& max);
Point2D FindRandomLocation(const GameInfo& game_info);
Point2D FindCenterOfMap(const GameInfo& game_info);

}