#pragma once

#include "s2clientprotocol/sc2api.pb.h"
#include "sc2_map_info.h"
#include "sc2_score.h"
#include "sc2_unit.h"
#include "sc2_action.h"

namespace sc2 {

typedef MessageResponsePtr<SC2APIProtocol::ResponseObservation> ResponseObservationPtr;
typedef MessageResponsePtr<SC2APIProtocol::Observation> ObservationPtr;
typedef MessageResponsePtr<SC2APIProtocol::ObservationRaw> ObservationRawPtr;
typedef MessageResponsePtr<SC2APIProtocol::ObservationRender> ObservationRenderPtr;
typedef MessageResponsePtr<SC2APIProtocol::ResponsePing> ResponsePingPtr;
typedef MessageResponsePtr<SC2APIProtocol::ResponseGameInfo> ResponseGameInfoPtr;
typedef MessageResponsePtr<SC2APIProtocol::ResponseQuery> ResponseQueryPtr;

bool Convert(const ObservationPtr& observation_ptr, Score& score);
bool Convert(const ObservationRawPtr& observation_ptr, UnitPool& unit_pool, uint32_t game_loop);
bool Convert(const ObservationPtr& observation_ptr, RenderedFrame& render);
bool Convert(const ResponseGameInfoPtr& response_game_info_ptr, GameInfo& game_info);

void ConvertRawActions(const ResponseObservationPtr& response_observation_ptr, RawActions& actions);
void ConvertFeatureLayerActions(const ResponseObservationPtr& response_observation_ptr, SpatialActions& actions);
void ConvertRenderedActions(const ResponseObservationPtr& response_observation_ptr, SpatialActions& actions);

Race ConvertRaceFromProto(SC2APIProtocol::Race race);
GameResult ConvertGameResultFromProto(SC2APIProtocol::Result result);
PlayerType ConvertPlayerTypeFromProto(SC2APIProtocol::PlayerType type);
Difficulty ConvertDifficultyFromProto(SC2APIProtocol::Difficulty difficulty);

}
