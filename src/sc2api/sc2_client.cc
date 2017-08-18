#include "sc2api/sc2_client.h"
#include "sc2api/sc2_proto_interface.h"
#include "sc2api/sc2_interfaces.h"
#include "sc2api/sc2_control_interfaces.h"
#include "sc2api/sc2_proto_to_pods.h"
#include "sc2api/sc2_game_settings.h"

#include "sc2utils/sc2_manage_process.h"

#include <algorithm>
#include <iostream>
#include <unordered_map>
#include <cassert>
#include <limits>
#include <fstream>

#include "sc2api.pb.h"

namespace sc2 {

//-------------------------------------------------------------------------------------------------
// ObservationImp: An implementation of ObservationInterface.
//-------------------------------------------------------------------------------------------------

class ObservationImp : public ObservationInterface {
public:
    ProtoInterface& proto_;
    ObservationPtr& observation_;
    ResponseObservationPtr& response_;
    ControlInterface& control_;
    uint32_t player_id_;

    // Game state info.
    Units units_;
    Units units_previous_;
    UnitIdxMap units_previous_map_;
    mutable Units units_added_;
    mutable Units units_removed_;
    uint32_t current_game_loop_;
    uint32_t previous_game_loop;
    RawActions raw_actions_;
    SpatialActions feature_layer_actions_;
    std::vector<PowerSource> power_sources_;
    std::vector<UpgradeID> upgrades_;
    std::vector<UpgradeID> upgrades_previous_;
    std::set<Tag> units_created_;

    // Game info.
    mutable GameInfo game_info_;
    mutable bool game_info_cached_;

    // Player data.
    int32_t minerals_;
    int32_t vespene_;
    int32_t food_cap_;
    int32_t food_used_;
    int32_t food_army_;
    int32_t food_workers_;
    int32_t idle_worker_count_;
    int32_t army_count_;
    int32_t warp_gate_count_;
    int32_t larva_count_;
    Point2D camera_pos_;
    Point3D start_location_;

    // Game data.
    mutable Abilities abilities_;
    mutable UnitTypes unit_types_;
    mutable Upgrades upgrade_ids_;
    mutable Buffs buff_ids_;

    // Score.
    Score score_;

    // Cached data.
    mutable bool abilities_cached_;
    mutable bool unit_types_cached;
    mutable bool upgrades_cached_;
    mutable bool buffs_cached_;

    ObservationImp(ProtoInterface& proto, ObservationPtr& observation, ResponseObservationPtr& response, ControlInterface& control);
    void ClearFlags();

    uint32_t GetPlayerID() const { return player_id_; }
    uint32_t GetGameLoop() const final { return current_game_loop_; }
    const Units& GetUnits() const final { return units_; }
    Units GetUnits(Filter filter) const final;
    Units GetUnits(Unit::Alliance alliance, Filter filter = {}) const final;
    const Unit* GetUnit(Tag tag) const final;
    const Unit* GetPreviousUnit(Tag tag) const final;
    const Units& GetUnitsAdded() const final;
    const Units& GetUnitsRemoved() const final;
    const RawActions& GetRawActions() const final { return raw_actions_; }
    const SpatialActions& GetFeatureLayerActions() const final { return feature_layer_actions_; };
    const std::vector<PowerSource>& GetPowerSources() const final { return power_sources_; }
    const std::vector<UpgradeID>& GetUpgrades() const final { return upgrades_; }
    const Score& GetScore() const final { return score_; }
    const Abilities& GetAbilityData(bool force_refresh = false) const final;
    const UnitTypes& GetUnitTypeData(bool force_refresh = false) const final;
    const Upgrades& GetUpgradeData(bool force_refresh = false) const final;
    const Buffs& GetBuffData(bool force_refresh = false) const final;
    const GameInfo& GetGameInfo() const final;
    bool HasCreep(const Point2D& point) const final;
    Visibility GetVisibility(const Point2D& point) const final;
    bool IsPathable(const Point2D& point) const final;
    bool IsPlacable(const Point2D& point) const final;
    float TerrainHeight(const Point2D& point) const final;

    int32_t GetMinerals() const final { return minerals_; }
    int32_t GetVespene() const final { return vespene_;  }
    int32_t GetFoodCap() const final { return food_cap_; }
    int32_t GetFoodUsed() const final { return food_used_; }
    int32_t GetFoodArmy() const final { return food_army_; }
    int32_t GetFoodWorkers() const final { return food_workers_; }
    int32_t GetIdleWorkerCount() const final { return idle_worker_count_; }
    int32_t GetArmyCount() const final { return army_count_; }
    int32_t GetWarpGateCount() const final { return warp_gate_count_; }
    Point2D GetCameraPos() const final { return camera_pos_; }
    Point3D GetStartLocation() const final { return start_location_; }

    const SC2APIProtocol::Observation* GetRawObservation() const final;

    bool UpdateObservation();
};

ObservationImp::ObservationImp(ProtoInterface& proto, ObservationPtr& observation, ResponseObservationPtr& response, ControlInterface& control) :
    proto_(proto),
    observation_(observation),
    response_(response),
    control_(control),
    current_game_loop_(std::numeric_limits<uint32_t>::max()),
    previous_game_loop(std::numeric_limits<uint32_t>::max()) {
    ClearFlags();
}

void ObservationImp::ClearFlags() {
    player_id_ = 0;
    game_info_cached_ = false;
    abilities_cached_ = false;
    unit_types_cached = false;
}

const Unit* ObservationImp::GetUnit(Tag tag) const {
    for (const Unit& unit : units_) {
        if (unit.tag == tag) {
            return &unit;
        }
    }

    return nullptr;
}

const Unit* ObservationImp::GetPreviousUnit(Tag tag) const {
    for (const Unit& unit : units_previous_) {
        if (unit.tag == tag) {
            return &unit;
        }
    }

    return nullptr;
}

Units ObservationImp::GetUnits(Unit::Alliance alliance, Filter filter) const {
    Units units;
    for (auto& u : units_) {
        if (u.alliance == alliance) {
            if (!filter) {
                units.push_back(u);
            }
            else if (filter(u)) {
                units.push_back(u);
            }
        }
    }
    return units;
}

Units ObservationImp::GetUnits(Filter filter) const {
    Units units;
    for (auto& u : units_) {
        if (!filter) {
            units.push_back(u);
        }
        else if (filter(u)) {
            units.push_back(u);
        }
    }
    return units;
}

const Units& ObservationImp::GetUnitsAdded() const {
    units_added_.clear();

    // Create the searchable set of previous units.
    std::set<Tag> previous_set;
    for (const Unit& unit : units_previous_) {
        previous_set.insert(unit.tag);
    }

    // If there are units not in the set, they are new.
    for (const Unit& unit : units_) {
        if (previous_set.find(unit.tag) == previous_set.end()) {
            units_added_.push_back(unit);
        }
    }

    return units_added_;
}

const Units& ObservationImp::GetUnitsRemoved() const {
    units_removed_.clear();

    // Create the searchable set of current units.
    std::set<Tag> current_set;
    for (const Unit& unit : units_) {
        current_set.insert(unit.tag);
    }

    // If there are units not in the set, they have been removed.
    for (const Unit& unit : units_previous_) {
        if (current_set.find(unit.tag) == current_set.end()) {
            units_removed_.push_back(unit);
        }
    }

    return units_removed_;
}

const Abilities& ObservationImp::GetAbilityData(bool force_refresh) const {
    if (force_refresh || abilities_.size() < 1) {
        abilities_cached_ = false;
    }

    if (abilities_cached_) {
        return abilities_;
    }

    abilities_.clear();

    // Send a request for ability ids.
    GameRequestPtr request = proto_.MakeRequest();
    SC2APIProtocol::RequestData* request_data = request->mutable_data();
    request_data->set_ability_id(true);

    if (!proto_.SendRequest(request)) {
        return abilities_;
    }

    GameResponsePtr response = control_.WaitForResponse();
    ResponseDataPtr response_data;
    SET_MESSAGE_RESPONSE(response_data, response, data);
    if (response_data.HasErrors()) {
        return abilities_;
    }

    if (response_data.HasErrors() || response_data->abilities_size() == 0) {
        return abilities_;
    }

    abilities_.resize(response_data->abilities_size());
    for (int i = 0; i < response_data->abilities_size(); ++i) {
        AbilityData& ability_data = abilities_[i];
        ability_data.ability_id = i;
        ability_data.remaps_from_ability_id.clear();
        ability_data.ReadFromProto(response_data->abilities(i));
    }

    for (AbilityData& ability_data : abilities_) {
        if (ability_data.remaps_to_ability_id == 0)
            continue;

        if (ability_data.remaps_to_ability_id >= abilities_.size()) {
            control_.Error(ClientError::InvalidAbilityRemap);
            ability_data.remaps_to_ability_id = 0;
            continue;
        }

        abilities_[ability_data.remaps_to_ability_id].remaps_from_ability_id.push_back(ability_data.ability_id);
    }

    abilities_cached_ = true;
    return abilities_;
}

const UnitTypes& ObservationImp::GetUnitTypeData(bool force_refresh) const {
    if (force_refresh || unit_types_.size() < 1) {
        unit_types_cached = false;
    }

    if (unit_types_cached) {
        return unit_types_;
    }

    unit_types_.clear();

    // Send a request for ability ids.
    GameRequestPtr request = proto_.MakeRequest();
    SC2APIProtocol::RequestData* request_data = request->mutable_data();
    request_data->set_unit_type_id(true);

    if (!proto_.SendRequest(request)) {
        return unit_types_;
    }

    GameResponsePtr response = control_.WaitForResponse();
    ResponseDataPtr response_data;
    SET_MESSAGE_RESPONSE(response_data, response, data);
    if (response_data.HasErrors()) {
        return unit_types_;
    }

    if (response_data.HasErrors() || response_data->units_size() == 0) {
        return unit_types_;
    }

    unit_types_.resize(response_data->units_size());
    for (int i = 0; i < response_data->units_size(); ++i) {
        UnitTypeData& unit = unit_types_[i];
        unit.unit_type_id = i;
        unit.ReadFromProto(response_data->units(i));
    }

    unit_types_cached = true;
    return unit_types_;
}

const Upgrades& ObservationImp::GetUpgradeData(bool force_refresh) const {
    if (force_refresh || upgrade_ids_.size() < 1) {
        upgrades_cached_ = false;
    }

    if (upgrades_cached_) {
        return upgrade_ids_;
    }

    upgrade_ids_.clear();

    GameRequestPtr request = proto_.MakeRequest();
    SC2APIProtocol::RequestData* request_data = request->mutable_data();
    request_data->set_upgrade_id(true);

    if (!proto_.SendRequest(request)) {
        return upgrade_ids_;
    }

    GameResponsePtr response = control_.WaitForResponse();
    ResponseDataPtr response_data;
    SET_MESSAGE_RESPONSE(response_data, response, data);
    if (response_data.HasErrors()) {
        return upgrade_ids_;
    }

    if (response_data.HasErrors() || response_data->upgrades_size() == 0) {
        return upgrade_ids_;
    }

    upgrade_ids_.resize(response_data->upgrades_size());
    for (int i = 0; i < response_data->upgrades_size(); ++i) {
        UpgradeData& upgrade = upgrade_ids_[i];
        upgrade.upgrade_id = i;
        upgrade.ReadFromProto(response_data->upgrades(i));
    }

    upgrades_cached_ = true;
    return upgrade_ids_;
}

const Buffs& ObservationImp::GetBuffData(bool force_refresh) const {
    if (force_refresh || buff_ids_.size() < 1) {
        buffs_cached_ = false;
    }

    if (buffs_cached_) {
        return buff_ids_;
    }

    buff_ids_.clear();

    GameRequestPtr request = proto_.MakeRequest();
    SC2APIProtocol::RequestData* request_data = request->mutable_data();
    request_data->set_buff_id(true);

    if (!proto_.SendRequest(request)) {
        return buff_ids_;
    }

    GameResponsePtr response = control_.WaitForResponse();
    ResponseDataPtr response_data;
    SET_MESSAGE_RESPONSE(response_data, response, data);
    if (response_data.HasErrors()) {
        return buff_ids_;
    }

    if (response_data.HasErrors() || response_data->buffs_size() == 0) {
        return buff_ids_;
    }

    buff_ids_.resize(response_data->buffs_size());
    for (int i = 0; i < response_data->buffs_size(); ++i) {
        BuffData& buff = buff_ids_[i];
        buff.buff_id = i;
        buff.ReadFromProto(response_data->buffs(i));
    }

    buffs_cached_ = true;
    return buff_ids_;
}

const GameInfo& ObservationImp::GetGameInfo() const {
    if (game_info_cached_) {
        return game_info_;
    }

    GameRequestPtr request = proto_.MakeRequest();
    request->mutable_game_info();

    if (!proto_.SendRequest(request)) {
        return game_info_;
    }

    GameResponsePtr response = control_.WaitForResponse();
    ResponseGameInfoPtr response_game_info;
    SET_MESSAGE_RESPONSE(response_game_info, response, game_info);
    if (response_game_info.HasErrors()) {
        return game_info_;
    }

    Convert(response_game_info, game_info_);

    game_info_cached_ = true;
    return game_info_;
}

static bool SampleImageData(const std::string& data, int width, int height, const Point2D& point, unsigned char& result) {
    Point2DI pointI(int(point.x), int(point.y));
    if (pointI.x < 0 || pointI.x >= width || pointI.y < 0 || pointI.y >= height) {
        return false;
    }

    // Image data is stored with an upper left origin.
    assert(data.size() == width * height);
    result = data[pointI.x + (height - 1 - pointI.y) * width];
    return true;
}

static bool SampleImageData(const SC2APIProtocol::ImageData& data, const Point2D& point, unsigned char& result) {
    return SampleImageData(data.data(), data.size().x(), data.size().y(), point, result);
}

static bool SampleImageData(const ImageData& data, const Point2D& point, unsigned char& result) {
    return SampleImageData(data.data, data.width, data.height, point, result);
}

bool ObservationImp::HasCreep(const Point2D& point) const {
    ObservationRawPtr observation_raw;
    SET_SUBMESSAGE_RESPONSE(observation_raw, observation_, raw_data);
    if (observation_raw.HasErrors()) {
        return false;
    }

    const SC2APIProtocol::MapState& map_state = observation_raw->map_state();
    const SC2APIProtocol::ImageData& creep = map_state.creep();

    unsigned char value;
    if (!SampleImageData(creep, point, value))
        return false;

    return value > 0 ? true : false;
}

Visibility ObservationImp::GetVisibility(const Point2D& point) const {
    ObservationRawPtr observation_raw;
    SET_SUBMESSAGE_RESPONSE(observation_raw, observation_, raw_data);
    if (observation_raw.HasErrors()) {
        return Visibility::FullHidden;
    }

    const SC2APIProtocol::MapState& map_state = observation_raw->map_state();
    const SC2APIProtocol::ImageData& visibility = map_state.visibility();

    unsigned char value;
    if (!SampleImageData(visibility, point, value))
        return Visibility::FullHidden;

    if (value == 0)
        return Visibility::Hidden;
    else if (value == 1)
        return Visibility::Fogged;
    else if (value == 2)
        return Visibility::Visible;
    else
        return Visibility::FullHidden;
}

bool ObservationImp::IsPathable(const Point2D& point) const {
    const GameInfo& game_info = GetGameInfo();

    unsigned char value;
    if (!SampleImageData(game_info.pathing_grid, point, value))
        return false;

    return value == 255 ? false : true;
}

bool ObservationImp::IsPlacable(const Point2D& point) const {
    const GameInfo& game_info = GetGameInfo();

    unsigned char value;
    if (!SampleImageData(game_info.placement_grid, point, value))
        return false;

    return value == 255 ? true : false;
}

float ObservationImp::TerrainHeight(const Point2D& point) const {
    const GameInfo& game_info = GetGameInfo();

    unsigned char value;
    if (!SampleImageData(game_info.terrain_height, point, value))
        return false;

    float decodedHeight = -100.0f + 200.0f * float(value) / 255.0f;
    return decodedHeight;
}

bool ObservationImp::UpdateObservation() {
    // Convert observation into data.
    if (!Convert(observation_, score_)) {
        return false;
    }

    uint32_t next_game_loop = observation_->game_loop();
    bool is_new_frame = next_game_loop != current_game_loop_;
    previous_game_loop = current_game_loop_;
    current_game_loop_ = next_game_loop;

    const SC2APIProtocol::PlayerCommon& player_common = observation_->player_common();
    assert(player_common.has_player_id());
    if (player_common.has_player_id()) {
        player_id_ = player_common.player_id();
    }

    // Fill out game data.
    minerals_ = player_common.minerals();
    vespene_ = player_common.vespene();
    food_used_ = player_common.food_used();
    food_cap_ = player_common.food_cap();
    food_army_ = player_common.food_army();
    food_workers_ = player_common.food_workers();
    idle_worker_count_ = player_common.idle_worker_count();
    army_count_ = player_common.army_count();
    warp_gate_count_ = player_common.warp_gate_count();
    larva_count_ = player_common.larva_count();

    // Actions first, as the actions apply to the previous selection.
    if (is_new_frame) {
        raw_actions_.clear();
        feature_layer_actions_ = SpatialActions();
    }

    Convert(response_, raw_actions_, units_, player_id_);
    Convert(response_, feature_layer_actions_, units_, player_id_);

    // Remap ability ids.
    {
        for (ActionRaw& action : raw_actions_) {
            action.ability_id = GetGeneralizedAbilityID(action.ability_id, *this);
        }
        for (SpatialUnitCommand& spatial_action : feature_layer_actions_.unit_commands) {
            spatial_action.ability_id = GetGeneralizedAbilityID(spatial_action.ability_id, *this);
        }
    }

    // Get units.
    if (is_new_frame) {
        units_previous_map_.clear();
        for (size_t i = 0, e = units_.size(); i < e; ++i) {
            units_previous_map_[units_[i].tag] = i;
        }
        units_previous_ = units_;
    }

    ObservationRawPtr observation_raw;
    SET_SUBMESSAGE_RESPONSE(observation_raw, observation_, raw_data);
    if (observation_raw.HasErrors()) {
        return false;
    }

    Convert(observation_raw, units_);

    // Remap ability ids in orders.
    for (Unit& unit : units_) {
        for (UnitOrder& unit_order : unit.orders) {
            unit_order.ability_id = GetGeneralizedAbilityID(unit_order.ability_id, *this);
        }
    }

    if (!observation_raw->has_player()) {
        return false;
    }

    const SC2APIProtocol::PlayerRaw& player_raw = observation_raw->player();
    if (!player_raw.has_camera()) {
        return false;
    }

    camera_pos_.x = player_raw.camera().x();
    camera_pos_.y = player_raw.camera().y();

    power_sources_.clear();
    for (int i = 0, e = player_raw.power_sources_size(); i < e; ++i) {
        const SC2APIProtocol::PowerSource& power_source = player_raw.power_sources(i);
        power_sources_.push_back(
            PowerSource(Point2D(power_source.pos().x(), power_source.pos().y()), power_source.radius(), power_source.tag()));
    }

    upgrades_previous_ = upgrades_;
    upgrades_.clear();
    for (int i = 0; i < player_raw.upgrade_ids_size(); ++i) {
        upgrades_.push_back(player_raw.upgrade_ids(i));
    }

    return true;
}

const SC2APIProtocol::Observation* ObservationImp::GetRawObservation() const {
    return observation_.get();
}


//-------------------------------------------------------------------------------------------------
// QueryImp: An implementation of QueryInterface.
//-------------------------------------------------------------------------------------------------

class QueryImp : public QueryInterface {
public:
    ProtoInterface& proto_;
    ControlInterface& control_;
    ObservationInterface& observation_;

    QueryImp(ProtoInterface& proto, ControlInterface& control, ObservationInterface& observation);

    AvailableAbilities GetAbilitiesForUnit(Tag tag, bool ignore_resource_requirements) final;
    std::vector<AvailableAbilities> GetAbilitiesForUnits(const std::vector<Tag>& tags, bool ignore_resource_requirements) final;

    float PathingDistance(const Point2D& start, const Point2D& end) final;
    float PathingDistance(const Tag& start_unit_tag, const Point2D& end) final;
    std::vector<float> PathingDistance(const std::vector<PathingQuery>& queries) final;

    bool Placement(const AbilityID& ability, const Point2D& target_pos, Tag placing_unit_tag_ = NullTag) final;
    std::vector<bool> Placement(const std::vector<PlacementQuery>& queries) final;
};

QueryImp::QueryImp(ProtoInterface& proto, ControlInterface& control, ObservationInterface& observation) :
    proto_(proto),
    control_(control),
    observation_(observation) {
}

AvailableAbilities QueryImp::GetAbilitiesForUnit(Tag tag, bool ignore_resource_requirements) {
    std::vector<Tag> tags;
    tags.push_back(tag);
    std::vector<AvailableAbilities> available_abilities = GetAbilitiesForUnits(tags, ignore_resource_requirements);
    control_.ErrorIf(available_abilities.empty(), ClientError::NoAbilitiesForTag);
    if (available_abilities.size() < 1) {
        return AvailableAbilities();
    }
    return available_abilities[0];
}

std::vector<AvailableAbilities> QueryImp::GetAbilitiesForUnits(const std::vector<Tag>& tags, bool ignore_resource_requirements) {
    std::vector<AvailableAbilities> available_abilities_out;

    // Make the request.
    {
        if (tags.empty()) {
            return available_abilities_out;
        }

        GameRequestPtr request = proto_.MakeRequest();
        SC2APIProtocol::RequestQuery* query = request->mutable_query();
        query->set_ignore_resource_requirements(ignore_resource_requirements);
        for (auto tag : tags) {
            SC2APIProtocol::RequestQueryAvailableAbilities* request_abilities = query->add_abilities();
            request_abilities->set_unit_tag(tag);
        }

        if (!proto_.SendRequest(request)) {
            return available_abilities_out;
        }
    }

    // Process the response.
    GameResponsePtr response = control_.WaitForResponse();
    if (!response.get()) {
        return available_abilities_out;
    }
    if (!response->has_query()) {
        control_.Error(ClientError::InvalidResponse);
        return available_abilities_out;
    }
    const SC2APIProtocol::ResponseQuery& query = response->query();
    if (query.abilities_size() < 1) {
        return available_abilities_out;
    }

    for (int i = 0; i < query.abilities_size(); ++i) {
        const SC2APIProtocol::ResponseQueryAvailableAbilities& response_query_available_abilities = query.abilities(i);
        AvailableAbilities available_abilities_unit;
        available_abilities_unit.unit_tag = response_query_available_abilities.unit_tag();
        available_abilities_unit.unit_type_id = response_query_available_abilities.unit_type_id();
        control_.ErrorIf(response_query_available_abilities.unit_tag() != tags[i], ClientError::ErrorSC2);
        for (int j = 0; j < response_query_available_abilities.abilities_size(); ++j) {
            const SC2APIProtocol::AvailableAbility& ability = response_query_available_abilities.abilities(j);
            AvailableAbility available_ability;
            available_ability.ability_id = GetGeneralizedAbilityID(ability.ability_id(), observation_);
            available_ability.requires_point = ability.requires_point();
            available_abilities_unit.abilities.push_back(available_ability);
        }

        available_abilities_out.push_back(available_abilities_unit);
    }

    return available_abilities_out;
}

float QueryImp::PathingDistance(const Point2D& start, const Point2D& end) {
    std::vector<PathingQuery> queries;

    PathingQuery query;
    query.start_ = start;
    query.end_ = end;
    queries.push_back(query);

    std::vector<float> distances = PathingDistance(queries);
    return distances[0];
}

float QueryImp::PathingDistance(const Tag& start_unit_tag, const Point2D& end) {
    std::vector<PathingQuery> queries;

    PathingQuery query;
    query.start_unit_tag_ = start_unit_tag;
    query.end_ = end;
    queries.push_back(query);

    std::vector<float> distances = PathingDistance(queries);
    return distances[0];
}

std::vector<float> QueryImp::PathingDistance(const std::vector<PathingQuery>& queries) {
    GameRequestPtr request = proto_.MakeRequest();
    SC2APIProtocol::RequestQuery* request_query = request->mutable_query();

    for (const PathingQuery& query : queries) {
        SC2APIProtocol::RequestQueryPathing* pathing_query = request_query->add_pathing();
        if (query.start_unit_tag_) {
            pathing_query->set_unit_tag(query.start_unit_tag_);
        }
        else {
            SC2APIProtocol::Point2D* startPos = pathing_query->mutable_start_pos();
            startPos->set_x(query.start_.x);
            startPos->set_y(query.start_.y);
        }
        SC2APIProtocol::Point2D* endPos = pathing_query->mutable_end_pos();
        endPos->set_x(query.end_.x);
        endPos->set_y(query.end_.y);
    }

    if (!proto_.SendRequest(request)) {
        return std::vector<float>(queries.size(), 0.0f);
    }

    GameResponsePtr response = control_.WaitForResponse();
    ResponseQueryPtr response_query;
    SET_MESSAGE_RESPONSE(response_query, response, query);
    if (response_query.HasErrors()) {
        return std::vector<float>(queries.size(), 0.0f);
    }

    if (response_query->pathing_size() != queries.size()) {
        return std::vector<float>(queries.size(), 0.0f);
    }

    std::vector<float> distances;
    distances.reserve(queries.size());

    for (int i = 0; i < response_query->pathing_size(); ++i) {
        const SC2APIProtocol::ResponseQueryPathing& result = response_query->pathing(i);
        float distance = result.distance();
        distances.push_back(distance);
    }

    return distances;
}

bool QueryImp::Placement(const AbilityID& ability, const Point2D& target_pos, Tag placing_unit_tag) {
    std::vector<PlacementQuery> queries;

    PlacementQuery query;
    query.ability = ability;
    query.target_pos= target_pos;
    query.placing_unit_tag = placing_unit_tag;
    queries.push_back(query);

    std::vector<bool> results = Placement(queries);
    return results[0];
}

std::vector<bool> QueryImp::Placement(const std::vector<PlacementQuery>& queries) {
    GameRequestPtr request = proto_.MakeRequest();
    SC2APIProtocol::RequestQuery* request_query = request->mutable_query();

    for (const PlacementQuery& query : queries) {
        SC2APIProtocol::RequestQueryBuildingPlacement* placement_query = request_query->add_placements();

        placement_query->set_placing_unit_tag(query.placing_unit_tag);
        placement_query->set_ability_id(query.ability);

        SC2APIProtocol::Point2D* target = placement_query->mutable_target_pos();
        target->set_x(query.target_pos.x);
        target->set_y(query.target_pos.y);
    }

    if (!proto_.SendRequest(request)) {
        return std::vector<bool>(queries.size() ,false);
    }

    GameResponsePtr response = control_.WaitForResponse();
    ResponseQueryPtr response_query;
    SET_MESSAGE_RESPONSE(response_query, response, query);
    if (response_query.HasErrors()) {
        return std::vector<bool>(queries.size() ,false);
    }

    if (response_query->placements_size() != queries.size()) {
        return std::vector<bool>(queries.size() ,false);
    }

    std::vector<bool> results;
    results.reserve(queries.size());

    for (int i = 0; i < response_query->placements_size(); ++i) {
        const SC2APIProtocol::ResponseQueryBuildingPlacement& result = response_query->placements(i);
        results.push_back(result.result() == SC2APIProtocol::ActionResult::Success);
    }

    return results;
}


//-------------------------------------------------------------------------------------------------
// DebugImp: An implementation of DebugInterface.
//-------------------------------------------------------------------------------------------------

class DebugImp : public DebugInterface {
public:
    ProtoInterface& proto_;
    ObservationInterface& observation_;
    ControlInterface& control_;

    // Debug display.
    struct DebugText {
        std::string text;
        bool has_coords;
        bool is_3d;
        Point3D pt;
        Color color;
    };
    std::vector<DebugText> debug_text_;

    struct DebugLine {
        Point3D p0;
        Point3D p1;
        Color color;
    };
    std::vector<DebugLine> debug_line_;

    struct DebugBox {
        Point3D p_min;
        Point3D p_max;
        Color color;
    };
    std::vector<DebugBox> debug_box_;

    struct DebugSphere {
        Point3D p_;
        float r_;
        Color color_;
    };
    std::vector<DebugSphere> debug_sphere_;

    std::vector<SC2APIProtocol::DebugGameState> debug_state_;

    struct DebugSetUnitValue {
        enum class UnitValue {
            Energy,
            Life,
            Shields
        };
        UnitValue unit_value;
        float value;
        Tag tag;
    };
    std::vector<DebugSetUnitValue> debug_unit_values_;

    struct DebugUnit {
        UnitTypeID unit_type;
        Point2D pos;
        uint32_t player_id;
        uint32_t count;
    };
    std::vector<DebugUnit> debug_unit_;

    std::vector<Tag> debug_kill_tag_;

    bool has_move_camera;
    Point2D debug_move_camera_;

    bool app_test_set_;
    AppTest app_test_;
    int app_test_delay_ms_;
    bool endgame_surrender_;
    bool endgame_victory_;
    bool set_score_;
    float score_;

    DebugImp(ProtoInterface& proto, ObservationInterface& observation, ControlInterface& control);

    void DebugTextOut(const std::string& out, Color color = Colors::White) override;
    void DebugTextOut(const std::string& out, const Point2D& pt_virtual_2D, Color color = Colors::White) override;
    void DebugTextOut(const std::string& out, const Point3D& pt3D, Color color = Colors::White) override;
    void DebugLineOut(const Point3D& p0, const Point3D& p1, Color color = Colors::White) override;
    void DebugBoxOut(const Point3D& p_min, const Point3D& p_max, Color color = Colors::White) override;
    void DebugSphereOut(const Point3D& p, float r, Color color = Colors::White) override;
    void DebugCreateUnit(UnitTypeID unit_type, const Point2D& p, uint32_t player_id = 1, uint32_t count = 1) override;
    void DebugKillUnit(Tag tag) override;
    void DebugShowMap() override;
    void DebugEnemyControl() override;
    void DebugIgnoreFood() override;
    void DebugIgnoreResourceCost() override;
    void DebugGiveAllResources() override;
    void DebugGodMode() override;
    void DebugIgnoreMineral() override;
    void DebugNoCooldowns() override;
    void DebugGiveAllTech() override;
    void DebugGiveAllUpgrades() override;
    void DebugFastBuild() override;
    void DebugSetScore(float score) override;
    void DebugEndGame(bool victory) override;
    void DebugSetEnergy(float value, Tag tag) override;
    void DebugSetLife(float value, Tag tag) override;
    void DebugSetShields(float value, Tag tag) override;
    void DebugMoveCamera(const Point2D& pos) override;
    void DebugTestApp(AppTest app_test, int delay_ms) override;
    void SendDebug() override;
};

DebugImp::DebugImp(ProtoInterface& proto, ObservationInterface& observation, ControlInterface& control) :
    proto_(proto),
    observation_(observation),
    control_(control),
    has_move_camera(false),
    app_test_set_(false),
    endgame_surrender_(false),
    endgame_victory_(false),
    set_score_(false),
    score_(0.0f) {
}

void DebugImp::DebugTextOut(const std::string& out, Color color) {
    DebugText debug_text;
    debug_text.text = out;
    debug_text.has_coords = false;
    debug_text.color = color;
    debug_text_.push_back(debug_text);
}

void DebugImp::DebugTextOut(const std::string& out, const Point2D& pt_virtual_2D, Color color) {
    DebugText debug_text;
    debug_text.text = out;
    debug_text.has_coords = true;
    debug_text.is_3d = false;
    debug_text.pt.x = pt_virtual_2D.x;
    debug_text.pt.y = pt_virtual_2D.y;
    debug_text.color = color;
    debug_text_.push_back(debug_text);
}

void DebugImp::DebugTextOut(const std::string& out, const Point3D& pt3D, Color color) {
    DebugText debug_text;
    debug_text.text = out;
    debug_text.has_coords = true;
    debug_text.is_3d = true;
    debug_text.pt.x = pt3D.x;
    debug_text.pt.y = pt3D.y;
    debug_text.pt.z = pt3D.z;
    debug_text.color = color;
    debug_text_.push_back(debug_text);
}

void DebugImp::DebugLineOut(const Point3D& p0, const Point3D& p1, Color color) {
    DebugLine line;
    line.p0 = p0;
    line.p1 = p1;
    line.color = color;
    debug_line_.push_back(line);
}

void DebugImp::DebugBoxOut(const Point3D& p_min, const Point3D& p_max, Color color) {
    DebugBox box;
    box.p_min = p_min;
    box.p_max = p_max;
    box.color = color;
    debug_box_.push_back(box);
}

void DebugImp::DebugSphereOut(const Point3D& p, float r, Color color) {
    DebugSphere sphere;
    sphere.p_ = p;
    sphere.r_ = r;
    sphere.color_ = color;
    debug_sphere_.push_back(sphere);
}

void DebugImp::DebugShowMap() {
    debug_state_.push_back(SC2APIProtocol::DebugGameState::show_map);
}

void DebugImp::DebugEnemyControl() {
    debug_state_.push_back(SC2APIProtocol::DebugGameState::control_enemy);
}

void DebugImp::DebugIgnoreFood() {
    debug_state_.push_back(SC2APIProtocol::DebugGameState::food);
}

void DebugImp::DebugIgnoreResourceCost() {
    debug_state_.push_back(SC2APIProtocol::DebugGameState::free);
}

void DebugImp::DebugGiveAllResources() {
    debug_state_.push_back(SC2APIProtocol::DebugGameState::all_resources);
}

void DebugImp::DebugGodMode() {
    debug_state_.push_back(SC2APIProtocol::DebugGameState::god);
}

void DebugImp::DebugIgnoreMineral() {
    debug_state_.push_back(SC2APIProtocol::DebugGameState::minerals);
}

void DebugImp::DebugNoCooldowns() {
    debug_state_.push_back(SC2APIProtocol::DebugGameState::cooldown);
}

void DebugImp::DebugGiveAllTech() {
    debug_state_.push_back(SC2APIProtocol::DebugGameState::tech_tree);
}

void DebugImp::DebugGiveAllUpgrades() {
    debug_state_.push_back(SC2APIProtocol::DebugGameState::upgrade);
}

void DebugImp::DebugFastBuild() {
    debug_state_.push_back(SC2APIProtocol::DebugGameState::fast_build);
}

void DebugImp::DebugSetScore(float score) {
    set_score_ = true;
    score_ = score;
}

void DebugImp::DebugEndGame(bool victory) {
    if (victory) {
        endgame_surrender_ = false;
        endgame_victory_ = true;
    }
    else {
        endgame_surrender_ = true;
        endgame_victory_ = false;
    }
}

void DebugImp::DebugSetEnergy(float value, Tag tag) {
    DebugSetUnitValue unit_value;
    unit_value.unit_value = DebugSetUnitValue::UnitValue::Energy;
    unit_value.value = value;
    unit_value.tag = tag;
    debug_unit_values_.push_back(unit_value);
}

void DebugImp::DebugSetLife(float value, Tag tag) {
    DebugSetUnitValue unit_value;
    unit_value.unit_value = DebugSetUnitValue::UnitValue::Life;
    unit_value.value = value;
    unit_value.tag = tag;
    debug_unit_values_.push_back(unit_value);
}

void DebugImp::DebugSetShields(float value, Tag tag) {
    DebugSetUnitValue unit_value;
    unit_value.unit_value = DebugSetUnitValue::UnitValue::Shields;
    unit_value.value = value;
    unit_value.tag = tag;
    debug_unit_values_.push_back(unit_value);
}

void DebugImp::DebugCreateUnit(UnitTypeID unit_type, const Point2D& p, uint32_t player_id, uint32_t count) {
    DebugUnit create_unit;
    create_unit.unit_type = unit_type;
    create_unit.pos = p;
    create_unit.player_id = player_id;
    create_unit.count = count;
    debug_unit_.push_back(create_unit);
}

void DebugImp::DebugKillUnit(Tag tag) {
    debug_kill_tag_.push_back(tag);
}

void DebugImp::DebugMoveCamera(const Point2D& pos) {
    has_move_camera = true;
    debug_move_camera_ = pos;
}

void DebugImp::DebugTestApp(AppTest app_test, int delay_ms) {
    app_test_set_ = true;
    app_test_ = app_test;
    app_test_delay_ms_ = delay_ms;
}

void DebugImp::SendDebug() {
    GameRequestPtr request = proto_.MakeRequest();
    SC2APIProtocol::RequestDebug* request_debug = request->mutable_debug();

    for (const DebugText& entry : debug_text_) {
        SC2APIProtocol::DebugCommand* command = request_debug->add_debug();
        SC2APIProtocol::DebugText* debug_text = command->mutable_draw()->add_text();
        debug_text->set_text(entry.text);
        if (entry.has_coords) {
            if (entry.is_3d) {
                SC2APIProtocol::Point* pos = debug_text->mutable_world_pos();
                pos->set_x(entry.pt.x);
                pos->set_y(entry.pt.y);
                pos->set_z(entry.pt.z);
            }
            else {
                SC2APIProtocol::Point* pos = debug_text->mutable_virtual_pos();
                pos->set_x(entry.pt.x);
                pos->set_y(entry.pt.y);
            }
        }
        SC2APIProtocol::Color* color_text = debug_text->mutable_color();
        color_text->set_r(entry.color.r);
        color_text->set_g(entry.color.g);
        color_text->set_b(entry.color.b);
    }

    for (const DebugLine& line : debug_line_) {
        SC2APIProtocol::DebugCommand* command = request_debug->add_debug();
        SC2APIProtocol::DebugLine* debug_line = command->mutable_draw()->add_lines();
        SC2APIProtocol::Line* proto_line = debug_line->mutable_line();

        SC2APIProtocol::Point* p0 = proto_line->mutable_p0();
        p0->set_x(line.p0.x);
        p0->set_y(line.p0.y);
        p0->set_z(line.p0.z);

        SC2APIProtocol::Point* p1 = proto_line->mutable_p1();
        p1->set_x(line.p1.x);
        p1->set_y(line.p1.y);
        p1->set_z(line.p1.z);

        SC2APIProtocol::Color* color_line = debug_line->mutable_color();
        color_line->set_r(line.color.r);
        color_line->set_g(line.color.g);
        color_line->set_b(line.color.b);
    }

    for (const DebugBox& box : debug_box_) {
        SC2APIProtocol::DebugCommand* command = request_debug->add_debug();
        SC2APIProtocol::DebugBox* debug_box = command->mutable_draw()->add_boxes();

        SC2APIProtocol::Point* p_min = debug_box->mutable_min();
        p_min->set_x(box.p_min.x);
        p_min->set_y(box.p_min.y);
        p_min->set_z(box.p_min.z);

        SC2APIProtocol::Point* p_max = debug_box->mutable_max();
        p_max->set_x(box.p_max.x);
        p_max->set_y(box.p_max.y);
        p_max->set_z(box.p_max.z);

        SC2APIProtocol::Color* color_box = debug_box->mutable_color();
        color_box->set_r(box.color.r);
        color_box->set_g(box.color.g);
        color_box->set_b(box.color.b);
    }

    for (const DebugSphere& sphere : debug_sphere_) {
        SC2APIProtocol::DebugCommand* command = request_debug->add_debug();
        SC2APIProtocol::DebugSphere* debug_sphere = command->mutable_draw()->add_spheres();

        SC2APIProtocol::Point* p = debug_sphere->mutable_p();
        p->set_x(sphere.p_.x);
        p->set_y(sphere.p_.y);
        p->set_z(sphere.p_.z);

        debug_sphere->set_r(sphere.r_);

        SC2APIProtocol::Color* color_sphere = debug_sphere->mutable_color();
        color_sphere->set_r(sphere.color_.r);
        color_sphere->set_g(sphere.color_.g);
        color_sphere->set_b(sphere.color_.b);
    }

    for (const DebugSetUnitValue& set_unit_value : debug_unit_values_) {
        SC2APIProtocol::DebugCommand* command = request_debug->add_debug();
        SC2APIProtocol::DebugSetUnitValue* unit_value = command->mutable_unit_value();
        switch (set_unit_value.unit_value) {
        case DebugSetUnitValue::UnitValue::Energy:
            unit_value->set_unit_value(SC2APIProtocol::DebugSetUnitValue_UnitValue_Energy);
            break;
        case DebugSetUnitValue::UnitValue::Life:
            unit_value->set_unit_value(SC2APIProtocol::DebugSetUnitValue_UnitValue_Life);
            break;
        case DebugSetUnitValue::UnitValue::Shields:
            unit_value->set_unit_value(SC2APIProtocol::DebugSetUnitValue_UnitValue_Shields);
            break;
        }
        unit_value->set_value(set_unit_value.value);
        unit_value->set_unit_tag(set_unit_value.tag);
    }

    for (const SC2APIProtocol::DebugGameState& state : debug_state_) {
        SC2APIProtocol::DebugCommand* command = request_debug->add_debug();
        command->set_game_state(state);
    }

    for (const DebugUnit& unit : debug_unit_) {
        if (unit.count < 1) {
            continue;
        }

        SC2APIProtocol::DebugCommand* command = request_debug->add_debug();
        SC2APIProtocol::DebugCreateUnit* create_unit = command->mutable_create_unit();
        create_unit->set_unit_type(unit.unit_type);
        create_unit->set_owner(unit.player_id);
        SC2APIProtocol::Point2D* pt = create_unit->mutable_pos();
        pt->set_x(unit.pos.x);
        pt->set_y(unit.pos.y);
        create_unit->set_quantity(unit.count);
    }

    if (debug_kill_tag_.size() > 0) {
        SC2APIProtocol::DebugCommand* command = request_debug->add_debug();
        SC2APIProtocol::DebugKillUnit* debug_kill_unit = command->mutable_kill_unit();
        for (std::size_t i = 0; i < debug_kill_tag_.size(); ++i) {
            debug_kill_unit->add_tag(debug_kill_tag_[i]);
        }
    }

    if (app_test_set_) {
        SC2APIProtocol::DebugCommand* command = request_debug->add_debug();
        SC2APIProtocol::DebugTestProcess* test_process = command->mutable_test_process();
        test_process->set_test(static_cast<SC2APIProtocol::DebugTestProcess_Test>(app_test_));
        test_process->set_delay_ms(app_test_delay_ms_);
    }

    if (set_score_) {
        SC2APIProtocol::DebugCommand* command = request_debug->add_debug();
        SC2APIProtocol::DebugSetScore* set_score = command->mutable_score();
        set_score->set_score(score_);
    }
    set_score_ = false;
    score_ = 0.0f;

    if (endgame_surrender_) {
        SC2APIProtocol::DebugCommand* command = request_debug->add_debug();
        SC2APIProtocol::DebugEndGame* end_game = command->mutable_end_game();
        end_game->set_end_result(SC2APIProtocol::DebugEndGame_EndResult_Surrender);
    }
    endgame_surrender_ = false;

    if (endgame_victory_) {
        SC2APIProtocol::DebugCommand* command = request_debug->add_debug();
        SC2APIProtocol::DebugEndGame* end_game = command->mutable_end_game();
        end_game->set_end_result(SC2APIProtocol::DebugEndGame_EndResult_DeclareVictory);
    }
    endgame_victory_ = false;

    proto_.SendRequest(request);
    debug_text_.clear();
    debug_line_.clear();
    debug_box_.clear();
    debug_sphere_.clear();

    debug_state_.clear();
    debug_unit_.clear();
    debug_kill_tag_.clear();
    debug_unit_values_.clear();

    // Wait for the response.
    control_.WaitForResponse();

    if (has_move_camera) {
        GameRequestPtr camera_request = proto_.MakeRequest();
        SC2APIProtocol::RequestAction* request_action = camera_request->mutable_action();
        SC2APIProtocol::Action* action = request_action->add_actions();
        SC2APIProtocol::ActionRaw* action_raw = action->mutable_action_raw();
        SC2APIProtocol::ActionRawCameraMove* camera_move = action_raw->mutable_camera_move();

        SC2APIProtocol::Point* point = camera_move->mutable_center_world_space();
        point->set_x(debug_move_camera_.x);
        point->set_y(debug_move_camera_.y);

        has_move_camera = false;
        proto_.SendRequest(camera_request);
        control_.WaitForResponse();
    }
}


//-------------------------------------------------------------------------------------------------
// ControlImp: an implementation of ControlInterface.
//-------------------------------------------------------------------------------------------------

class ControlImp : public ControlInterface {
public:
    ControlImp(sc2::Client& client);
    ~ControlImp();

    sc2::Client& client_;
    AppState app_state_;

    bool is_multiplayer_;

    // Proto and socket interface to the game.
    ProtoInterface proto_;

    // Observation from last step.
    ObservationPtr observation_;
    ResponseObservationPtr response_;

    std::unique_ptr<ObservationImp> observation_imp_;
    std::unique_ptr<QueryImp> query_imp_;
    std::unique_ptr<DebugImp> debug_imp_;
    ProcessInfo pi_;

    // Errors that may have occured during calls to the various interfaces.
    std::vector<ClientError> client_errors_;
    std::vector<std::string> protocol_errors_;

    std::mutex error_mutex_;

    ProtoInterface& Proto() override;

    virtual bool RemoteSaveMap(const void* data, int data_size, std::string remote_path) override;
    bool Connect(const std::string& address, int port, int timeout_ms) override;
    bool CreateGame(const std::string& map_name, const std::vector<PlayerSetup>& players, bool realtime) override;

    bool RequestJoinGame(PlayerSetup setup, const InterfaceSettings& settings, const Ports& ports = Ports()) override;
    bool WaitJoinGame() override;

    bool RequestLeaveGame() override;
    bool PollLeaveGame() override;

    bool Step(int count = 1) override;
    bool WaitStep() override;

    bool SaveReplay(const std::string& path) override;

    bool Ping() override;

    GameResponsePtr WaitForResponse() override;
    void SetProcessInfo(const ProcessInfo& pi) override;
    const ProcessInfo& GetProcessInfo() const override;

    // Game status.
    SC2APIProtocol::Status GetLastStatus() const override;
    AppState GetAppState() const override;
    bool IsInGame() const override;
    bool IsFinishedGame() const override;
    bool IsReadyForCreateGame() const override;
    bool HasResponsePending() const override;

    bool GetObservation() override;
    bool PollResponse() override;
    bool ConsumeResponse() override;

    bool IssueEvents(const std::vector<Tag>& commands = {}) override;
    void OnGameStart() override;

    void Error(ClientError error, const std::vector<std::string>& errors = {}) override;
    void ErrorIf(bool condition, ClientError error, const std::vector<std::string>& errors = {}) override;

    void IssueUnitDestroyedEvents();
    void IssueUnitAddedEvents();
    void IssueIdleEvent(const Unit& unit, const std::vector<Tag>& commands);
    void IssueBuildingCompletedEvent(const Unit& unit);
    void IssueAlertEvents();
    void IssueUpgradeEvents();

    void DumpProtoUsage() override;

    void ResolveMap (const std::string& map_name, SC2APIProtocol::RequestCreateGame* request);

    const std::vector<ClientError>& GetClientErrors() const final { return client_errors_; };
    const std::vector<std::string>& GetProtocolErrors() const final { return protocol_errors_; };

    void ClearClientErrors() override { client_errors_.clear(); };
    void ClearProtocolErrors() override { protocol_errors_.clear(); };
};

ControlImp::ControlImp(Client& client) :
    client_(client),
    app_state_(AppState::normal),
    is_multiplayer_(false),
    observation_imp_(nullptr),
    query_imp_(nullptr),
    debug_imp_(nullptr) {
    proto_.SetControl(this);
    observation_imp_ = std::make_unique<ObservationImp>(proto_, observation_, response_, *this);
    query_imp_ = std::make_unique<QueryImp>(proto_, *this, *observation_imp_);
    debug_imp_= std::make_unique<DebugImp>(proto_, *observation_imp_, *this);
}

ControlImp::~ControlImp() {
    proto_.Quit();
}

ProtoInterface& ControlImp::Proto() {
    return proto_;
}

bool ControlImp::Connect(const std::string& address, int port, int timeout_ms) {
    // Keep retrying the connection until the timeout is hit.
    bool connected = false;
    unsigned int timeout_seconds = ((unsigned int)timeout_ms + 1500) / 1000;
    if (timeout_seconds < 1) {
        timeout_seconds = 1;
    }


    for (unsigned int count_seconds = 0; count_seconds < timeout_seconds; ++count_seconds) {
        if (proto_.ConnectToGame(address, port, timeout_ms)) {
            connected = true;
            break;
        }
        SleepFor(1000);

        if (count_seconds == 0) {
            std::cout << "Waiting for connection";
        } else {
            std::cout << ".";
        }
    }
    std::cout << std::endl;

    if (!connected) {
        std::cerr << "Unable to connect to game" << std::endl;
        return false;
    }

    std::cout << "Connected to " << address << ":" << port << std::endl;

    return true;
}

bool ControlImp::RemoteSaveMap(const void* data, int data_size, std::string remote_path) {
    // Request.
    {
        GameRequestPtr request = proto_.MakeRequest();
        SC2APIProtocol::RequestSaveMap* request_save_map = request->mutable_save_map();
        request_save_map->set_map_path(remote_path);
        request_save_map->set_map_data(data, data_size);

        if (!proto_.SendRequest(request)) {
            return false;
        }
    }

    // Response.
    GameResponsePtr response = WaitForResponse();
    if (!response.get()) {
        return false;
    }

    if (!response->has_save_map()) {
        std::cerr << "Error in ResponseSaveMap" << std::endl;
        return false;
    }
    const SC2APIProtocol::ResponseSaveMap& response_save_game = response->save_map();

    bool success = true;
    if (response_save_game.has_error()) {
        success = false;
        std::string errorCode = "Unknown";
        switch (response_save_game.error()) {
            case SC2APIProtocol::ResponseSaveMap::InvalidMapData: {
                errorCode = "Invalid Map Data";
                break;
            }
            default: {
                break;
            }
        }
    }

    return success;
}

void ControlImp::ResolveMap (const std::string& map_name, SC2APIProtocol::RequestCreateGame* request) {
    // BattleNet map
    if (!HasExtension(map_name, ".SC2Map")) {
        request->set_battlenet_map_name(map_name);
        return;
    }

    // Absolute path
    SC2APIProtocol::LocalMap* local_map = request->mutable_local_map();
    if (DoesFileExist(map_name)) {
        local_map->set_map_path(map_name);
        return;
    }

    // Relative path - Game maps directory
    std::string game_relative = GetGameMapsDirectory(pi_.process_path) + map_name;
    if (DoesFileExist(game_relative)) {
        local_map->set_map_path(map_name);
        return;
    }

    // Relative path - Library maps directory
    std::string library_relative = GetLibraryMapsDirectory() + map_name;
    if (DoesFileExist(library_relative)) {
        local_map->set_map_path(library_relative);
        return;
    }

    // Relative path - Remotely saved maps directory
    local_map->set_map_path(map_name);
}

bool ControlImp::CreateGame(const std::string& map_name, const std::vector<PlayerSetup>& players, bool realtime) {
    GameRequestPtr request = proto_.MakeRequest();
    SC2APIProtocol::RequestCreateGame* request_create_game = request->mutable_create_game();
    ResolveMap(map_name, request_create_game);
    for (const PlayerSetup& setup : players) {
        SC2APIProtocol::PlayerSetup* playerSetup = request_create_game->add_player_setup();
        playerSetup->set_type(SC2APIProtocol::PlayerType(setup.type));
        playerSetup->set_race(SC2APIProtocol::Race(int(setup.race) + 1));
        playerSetup->set_difficulty(SC2APIProtocol::Difficulty(setup.difficulty));
    }

    request_create_game->set_realtime(realtime);

    if (!proto_.SendRequest(request)) {
        return false;
    }

    GameResponsePtr response = WaitForResponse();
    if (!response.get()) {
        return false;
    }

    if (!response->has_create_game()) {
        std::cerr << "Error in ResponseCreateGame" << std::endl;
        return false;
    }

    const SC2APIProtocol::ResponseCreateGame& response_create_game = response->create_game();

    bool success = true;
    if (response_create_game.has_error()) {
        std::string errorCode = "Unknown";
        switch (response_create_game.error()) {
            case SC2APIProtocol::ResponseCreateGame::MissingMap: {
                errorCode = "Missing Map";
                break;
            }
            case SC2APIProtocol::ResponseCreateGame::InvalidMapPath: {
                errorCode = "Invalid Map Path";
                break;
            }
            case SC2APIProtocol::ResponseCreateGame::InvalidMapData: {
                errorCode = "Invalid Map Data";
                break;
            }
            case SC2APIProtocol::ResponseCreateGame::InvalidMapName: {
                errorCode = "Invalid Map Name";
                break;
            }
            case SC2APIProtocol::ResponseCreateGame::InvalidMapHandle: {
                errorCode = "Invalid Map Handle";
                break;
            }
            case SC2APIProtocol::ResponseCreateGame::MissingPlayerSetup: {
                errorCode = "Missing Player Setup";
                break;
            }
            case SC2APIProtocol::ResponseCreateGame::InvalidPlayerSetup: {
                errorCode = "Invalid Player Setup";
                break;
            }
            default: {
                break;
            }
        }

        std::cerr << "CreateGame request returned an error code: " << errorCode << std::endl;
        success = false;
    }

    if (response_create_game.has_error_details() && response_create_game.error_details().length() > 0) {
        std::cerr << "CreateGame request returned error details: " << response_create_game.error_details() << std::endl;
        success = false;
    }

    return success;
}

bool ControlImp::RequestJoinGame(PlayerSetup setup, const InterfaceSettings& settings, const Ports& ports) {
    observation_imp_->ClearFlags();

    is_multiplayer_ = ports.IsValid();

    GameRequestPtr request = proto_.MakeRequest();
    SC2APIProtocol::RequestJoinGame* request_join_game = request->mutable_join_game();

    request_join_game->set_race(SC2APIProtocol::Race(int(setup.race) + 1));
    if (is_multiplayer_) {
        // Set shared port.
        request_join_game->set_shared_port(ports.shared_port);

        // Set server ports.
        SC2APIProtocol::PortSet* server_ports = request_join_game->mutable_server_ports();
        server_ports->set_game_port(ports.server_ports.game_port);
        server_ports->set_base_port(ports.server_ports.base_port);

        // Set client ports. Right now only 1v1 is supported.
        for (const PortSet& client_ports : ports.client_ports) {
            SC2APIProtocol::PortSet* client_port = request_join_game->add_client_ports();
            client_port->set_game_port(client_ports.game_port);
            client_port->set_base_port(client_ports.base_port);
        }
    }

    SC2APIProtocol::InterfaceOptions* options = request_join_game->mutable_options();
    options->set_raw(true);
    options->set_score(true);
    if (settings.use_feature_layers) {
        SC2APIProtocol::SpatialCameraSetup* setupProto = options->mutable_feature_layer();
        setupProto->set_width(settings.feature_layer_settings.camera_width);
        SC2APIProtocol::Size2DI* resolution = setupProto->mutable_resolution();
        resolution->set_x(settings.feature_layer_settings.map_x);
        resolution->set_y(settings.feature_layer_settings.map_y);
        SC2APIProtocol::Size2DI* minimap_resolution = setupProto->mutable_minimap_resolution();
        minimap_resolution->set_x(settings.feature_layer_settings.minimap_x);
        minimap_resolution->set_y(settings.feature_layer_settings.minimap_y);
    }
    if (settings.use_render) {
        SC2APIProtocol::SpatialCameraSetup* setupProto = options->mutable_render();
        SC2APIProtocol::Size2DI* resolution = setupProto->mutable_resolution();
        resolution->set_x(settings.render_settings.map_x);
        resolution->set_y(settings.render_settings.map_y);
        SC2APIProtocol::Size2DI* minimap_resolution = setupProto->mutable_minimap_resolution();
        minimap_resolution->set_x(settings.render_settings.minimap_x);
        minimap_resolution->set_y(settings.render_settings.minimap_y);
    }

    if (!proto_.SendRequest(request)) {
        return false;
    }

    return true;
}

bool ControlImp::RequestLeaveGame() {
    if (!is_multiplayer_) {
        return false;
    }

    GameRequestPtr request = proto_.MakeRequest();
    request->mutable_leave_game();
    if (!proto_.SendRequest(request)) {
        return false;
    }

    return true;
}

bool ControlImp::PollLeaveGame() {
    if (!is_multiplayer_) {
        return false;
    }

    if (proto_.GetResponsePending() != SC2APIProtocol::Response::kLeaveGame) {
        // If not in a game, then it is in the end state trying to leave the game.
        ErrorIf(HasResponsePending(), ClientError::ResponseNotConsumed);
        return !IsInGame();
    }

    // React to receiving a leave response.
    if (!PollResponse()) {
        return true;
    }

    // Wait for the end response to be received before proceeding.
    // TODO: Add error handling.
    WaitForResponse();
    return true;
}

bool ControlImp::Step(int count) {
    if (app_state_ != AppState::normal)
        return false;

    GameRequestPtr request = proto_.MakeRequest();
    SC2APIProtocol::RequestStep* step = request->mutable_step();
    step->set_count(count);
    if (!proto_.SendRequest(request)) {
        return false;
    }

    return true;
}

bool ControlImp::WaitStep() {
    GameResponsePtr response = WaitForResponse();
    if (!response.get() || !response->has_step() || response->error_size() > 0) {
        return false;
    }

    return GetObservation();
}

bool ControlImp::SaveReplay(const std::string& path) {
    GameRequestPtr request = proto_.MakeRequest();
    request->mutable_save_replay();
    if (!proto_.SendRequest(request)) {
        return false;
    }

    GameResponsePtr response = WaitForResponse();
    if (!response.get() || !response->has_save_replay() || response->error_size() > 0) {
        return false;
    }

    const SC2APIProtocol::ResponseSaveReplay& response_replay = response->save_replay();

    if (response_replay.data().size() == 0) {
        return false;
    }

    std::ofstream file;
    file.open(path, std::fstream::binary);
    if (!file.is_open()) {
        return false;
    }

    file.write(&response_replay.data()[0], response_replay.data().size());
    return true;
}

bool ControlImp::Ping() {
    return proto_.PingGame();
}

GameResponsePtr ControlImp::WaitForResponse() {
    assert(app_state_ == AppState::normal);

    GameResponsePtr response = proto_.WaitForResponseInternal();

    if (response.get() && response->error_size() < 1) {
        // Everything is good. No need for any error handling.
        return response;
    }

    if (response.get() && response->error_size() > 0) {
        std::vector<std::string> errors;
        for (int i = 0; i < response->error_size(); ++i) {
            errors.push_back(response->error(i));
        }

        Error(ClientError::SC2ProtocolError, errors);
        return response;
    }
    assert(!response.get());

    // The game application did not responded, the previous request was either not sent or the app is non-responsive.

    // Step 1: distinguish between a hang and a crash. Lots of time has elapsed, so if there was a crash
    // it should have finished by now.
    assert(pi_.process_id);
    if (!IsProcessRunning(pi_.process_id)) {
        app_state_ = AppState::crashed;
        std::cout << "Game application has terminated unexpectedly." << std::endl;
        Error(ClientError::SC2AppFailure);
        return response;
    }

    // Step 2: distinguish between a non-responsive app and a failure to deliver a valid request.
    {
        GameRequestPtr ping_request = proto_.MakeRequest();
        ping_request->mutable_ping();

        if (!proto_.SendRequest(ping_request, true)) {
            // Mark the game app as unresponsive.
            app_state_ = AppState::timeout;
            Error(ClientError::SC2ProtocolTimeout);
        }
        else {
            // Wait for a ping response. If this fails, the game is unresponsive.
            // TODO: Implement a timeout parameter for this wait.
            GameResponsePtr response_ping = proto_.WaitForResponseInternal();
            if (response_ping) {
                if (proto_.GetLastStatus() == SC2APIProtocol::Status::unknown) {
                    Error(ClientError::SC2UnknownStatus);
                }

                // The game is responsive, but there was another problem. This isn't the right
                // place to handle another type of problem. Just return the nullptr.
                Error(ClientError::SC2UnknownStatus);
                return response;
            }

            app_state_ = AppState::timeout;
            Error(ClientError::SC2ProtocolTimeout);
        }
    }

    // The game application has hanged. Try and terminate it.
    app_state_ = AppState::timeout;
    for (int i = 0; i < 10 && IsProcessRunning(pi_.process_id); ++i) {
        TerminateProcess(pi_.process_id);
        SleepFor(2000);
    }

    if (IsProcessRunning(pi_.process_id)) {
        // Failed to kill the running process.
        app_state_ = AppState::timeout_zombie;
    }

    std::cout << "Game application has been terminated due to unresponsiveness." << std::endl;
    Error(ClientError::SC2AppFailure);
    return response;
}

void ControlImp::SetProcessInfo(const ProcessInfo& pi) {
    pi_ = pi;
}

const ProcessInfo& ControlImp::GetProcessInfo() const {
    return pi_;
}

SC2APIProtocol::Status ControlImp::GetLastStatus() const {
    return proto_.GetLastStatus();
}

AppState ControlImp::GetAppState() const {
    return app_state_;
}

bool ControlImp::IsInGame() const {
    if (app_state_ != AppState::normal)
        return false;

    return GetLastStatus() == SC2APIProtocol::Status::in_game || GetLastStatus() == SC2APIProtocol::Status::in_replay;
}

bool ControlImp::IsFinishedGame() const {
    if (app_state_ != AppState::normal)
        return true;

    if (IsInGame()) {
        return false;
    }

    if (HasResponsePending()) {
        return false;
    }

    return true;
}

bool ControlImp::IsReadyForCreateGame() const {
    if (app_state_ != AppState::normal)
        return false;

    // Make sure the pipes are clear first.
    if (HasResponsePending()) {
        return false;
    }

    // TODO: For multiplayer, it may be possible to be in the ended state but not yet left the game. Must leave the game
    // before create game can be ready again.
    return GetLastStatus() == SC2APIProtocol::Status::launched || GetLastStatus() == SC2APIProtocol::Status::ended;
}

bool ControlImp::HasResponsePending() const {
    return proto_.HasResponsePending();
}

bool ControlImp::GetObservation() {
    if (app_state_ != AppState::normal)
        return false;

    GameRequestPtr request = proto_.MakeRequest();
    request->mutable_observation();
    if (!proto_.SendRequest(request)) {
        return false;
    }

    GameResponsePtr response = WaitForResponse();
    ResponseObservationPtr response_observation;
    SET_MESSAGE_RESPONSE(response_observation, response, observation);
    if (response_observation.HasErrors()) {
        std::cerr << std::endl << "Error in returning observation:" << std::endl;
        std::cerr << "The main response is of type: " << std::to_string(response->response_case()) << std::endl;
        if (response_observation.HasResponse()) {
            std::cerr << "There is no ResponseObservation/message!" << std::endl;
        }
        if (response->error_size() > 0) {
            for (int i = 0; i < response->error_size(); ++i)
                std::cerr << "Error string: " << response->error(i) << std::endl;
        }
        else {
            std::cerr << "No error strings in result." << std::endl;
        }
        std::cerr << std::endl;
        return false;
    }

    ObservationPtr observation;
    SET_SUBMESSAGE_RESPONSE(observation, response_observation, observation);
    if (observation.HasErrors()) {
        return false;
    }

    observation_ = observation;
    response_ = response_observation;

    observation_imp_->UpdateObservation();

    return true;
}

bool ControlImp::WaitJoinGame() {
    std::cout << "Waiting for the JoinGame response." << std::endl;
    GameResponsePtr response = WaitForResponse();
    if (!response.get()) {
        std::cout << "Did not get a JoinGame response." << std::endl;
        return false;
    }
    if (!response->has_join_game()) {
        std::cout << "Response received is not JoinGame response." << std::endl;
        return false;
    }

    if (response->error_size() > 0) {
        std::cout << "Error in joining the game." << std::endl;
        assert(0);
        return false;
    }

    observation_imp_->player_id_ = response->join_game().player_id();

    std::cout << "WaitJoinGame finished successfully." << std::endl;
    return true;
}

bool ControlImp::PollResponse() {
    return proto_.PollResponse();
}

bool ControlImp::ConsumeResponse() {
    GameResponsePtr response = WaitForResponse();
    if (!response.get()) {
        return false;
    }

    return true;
}

void ControlImp::IssueUnitDestroyedEvents() {
    if (!observation_->has_raw_data()) {
        return;
    }

    const SC2APIProtocol::ObservationRaw& raw = observation_->raw_data();
    if (raw.has_event()) {
        const SC2APIProtocol::Event& event = raw.event();
        for (const auto& tag : event.dead_units()) {
            const Unit* u = observation_imp_->GetPreviousUnit(tag);
            if (!u) {
                continue;
            }
            client_.OnUnitDestroyed(*u);
        }
    }
}

void ControlImp::IssueUnitAddedEvents() {
    const Units& added = observation_imp_->GetUnitsAdded();
    for (const auto& add : added) {
        if (add.alliance == Unit::Alliance::Self) {
            std::set<Tag>::iterator it_found = observation_imp_->units_created_.find(add.tag);
            if (it_found == observation_imp_->units_created_.end()) {
                client_.OnUnitCreated(add);
                observation_imp_->units_created_.insert(add.tag);
            }
        }

        if (add.alliance == Unit::Alliance::Enemy && add.display_type == Unit::DisplayType::Visible) {
            client_.OnUnitEnterVision(add);
        }
    }
}

void ControlImp::IssueIdleEvent(const Unit& unit, const std::vector<Tag>& commands) {
    if (!unit.orders.empty() || unit.build_progress < 1.0f) {
        return;
    }
    // Lookup unit from previous map.
    UnitIdxMap::const_iterator found = observation_imp_->units_previous_map_.find(unit.tag);

    // If it's not in the previous map it's a new unit with new orders so trigger the OnIdle event.
    if (found == observation_imp_->units_previous_map_.end()) {
        client_.OnUnitIdle(unit);
        return;
    }

    // Otherwise get that unit from the previous list and verify it's state changed to idle.
    const Unit& unit_previous = observation_imp_->units_previous_[found->second];

    if (!unit_previous.orders.empty()) {
        client_.OnUnitIdle(unit);
        return;
    }

    // If the unit had less than 1.0 build progress in the last stop this is the first time it's active.
    if (unit_previous.build_progress < 1.0f) {
        client_.OnUnitIdle(unit);
        return;
    }

    // Iterate the issued commands, if a unit exists in that list but does not currently have orders
    // the order must have failed. Reissue the OnUnitIdle event in that case.
    for (auto t : commands) {
        if (t == unit.tag) {
            client_.OnUnitIdle(unit);
            break;
        }
    }
}

void ControlImp::IssueBuildingCompletedEvent(const Unit& unit) {
    // If the units build progress is complete but it previously wasn't call construction complete
    if (unit.build_progress < 1.0f) {
        return;
    }

    UnitIdxMap::const_iterator found = observation_imp_->units_previous_map_.find(unit.tag);

    if (found == observation_imp_->units_previous_map_.end()) {
        return;
    }

    const Unit& unit_previous = observation_imp_->units_previous_[found->second];
    if (unit_previous.build_progress < 1.0f) {
        client_.OnBuildingConstructionComplete(unit);
    }
}

void ControlImp::IssueAlertEvents() {
    // Iterate the alerts and issue relevant events.
    for (const auto alert : observation_->alerts()) {
        switch (alert) {
            case SC2APIProtocol::Alert::NuclearLaunchDetected: {
                client_.OnNuclearLaunchDetected();
                break;
            }
            case SC2APIProtocol::Alert::NydusWormDetected: {
                client_.OnNydusDetected();
                break;
            }
            default: {
                break;
            }
        }
    }
}

void ControlImp::IssueUpgradeEvents() {
    std::set<uint32_t> previous;
    for (auto up : observation_imp_->upgrades_previous_) {
        previous.insert(up);
    }

    for (auto up : observation_imp_->upgrades_) {
        if (previous.find(up) == previous.end()) {
            client_.OnUpgradeCompleted(up);
        }
    }
}

bool ControlImp::IssueEvents(const std::vector<Tag>& commands) {
    if (observation_imp_->current_game_loop_ == observation_imp_->previous_game_loop) {
        return false;
    }

    IssueUnitDestroyedEvents();
    IssueUnitAddedEvents();

    const Units& units = observation_imp_->GetUnits(Unit::Alliance::Self);
    for (const auto& unit : units) {
        IssueIdleEvent(unit, commands);
        IssueBuildingCompletedEvent(unit);
    }

    IssueUpgradeEvents();
    IssueAlertEvents();

    // Run the users OnStep function after events have been issued.
    client_.OnStep();

    return true;
}

void ControlImp::OnGameStart() {
    const Units& units = observation_imp_->GetUnits(Unit::Alliance::Self, [](const Unit& unit) {
        return unit.unit_type == UNIT_TYPEID::TERRAN_COMMANDCENTER ||
                unit.unit_type == UNIT_TYPEID::PROTOSS_NEXUS ||
                unit.unit_type == UNIT_TYPEID::ZERG_HATCHERY;
    });

    observation_imp_->units_created_.clear();

    if (units.empty()) {
        return;
    }

    // For now, until the api supports allies, the first (and only) building in this list should be the start location
    observation_imp_->start_location_ = units.begin()->pos;
}

void ControlImp::Error(ClientError error, const std::vector<std::string>& errors) {
    // An ConnectionClosed error can come off a civetweb worker thread.
    std::lock_guard<std::mutex> guard(error_mutex_);

    // Cache all the errors that happen
    client_errors_.push_back(error);
    for (auto const& e : errors) {
        protocol_errors_.push_back(e);
    }

#ifdef SC2API_ASSERT_ON_ERROR
    assert(0);
#endif
}

void ControlImp::ErrorIf(bool condition, ClientError error, const std::vector<std::string>& errors) {
    if (condition) {
        Error(error, errors);
    }
}

void ControlImp::DumpProtoUsage() {
    const std::vector<uint32_t>& stats = proto_.GetStats();
    std::cout << "******************************************************" << std::endl;
    std::cout << "Protocol use by message type:" << std::endl;
    for (std::size_t i = 0; i < stats.size(); ++i) {
        if (stats[i] == 0)
            continue;

        std::cout << std::to_string(i) << ": " << std::to_string(stats[i]) << std::endl;
    }

    std::cout << "******************************************************" << std::endl;
}

//-------------------------------------------------------------------------------------------------
// Client
//-------------------------------------------------------------------------------------------------

Client::Client() :
    control_imp_(nullptr) {
    control_imp_ = new ControlImp(*this);
}

Client::~Client() {
    delete control_imp_;
}

const ObservationInterface* Client::Observation() const {
    // TODO: Should this return a nullptr if the interface is not valid (e.g., before a game is started)?
    return control_imp_->observation_imp_.get();
}

QueryInterface* Client::Query() {
    // TODO: Should this return a nullptr if the interface is not valid (e.g., before a game is started)?
    return control_imp_->query_imp_.get();
}

DebugInterface* Client::Debug() {
    return control_imp_->debug_imp_.get();
}

ControlInterface* Client::Control() {
    return control_imp_;
}

const ControlInterface* Client::Control() const {
    return control_imp_;
}

void Client::Reset() {
    delete control_imp_;
    control_imp_ = new ControlImp(*this);
}

bool IsCarryingMinerals(const Unit& unit) {
    auto is_mineral = [](const BuffID& buff){
        return buff == BUFF_ID::CARRYMINERALFIELDMINERALS
            || buff == BUFF_ID::CARRYHIGHYIELDMINERALFIELDMINERALS;
    };
    return std::find_if(unit.buffs.begin(), unit.buffs.end(), is_mineral) != unit.buffs.end();
}

bool IsCarryingVespene(const Unit& unit) {
    auto is_vespene = [](const BuffID& buff){
        return buff == BUFF_ID::CARRYHARVESTABLEVESPENEGEYSERGAS
            || buff == BUFF_ID::CARRYHARVESTABLEVESPENEGEYSERGASPROTOSS
            || buff == BUFF_ID::CARRYHARVESTABLEVESPENEGEYSERGASZERG;
    };
    return std::find_if(unit.buffs.begin(), unit.buffs.end(), is_vespene) != unit.buffs.end();
}

}
