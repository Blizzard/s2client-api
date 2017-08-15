#include "sc2api/sc2_proto_to_pods.h"

#include <iostream>
#include <cassert>


namespace sc2 {

typedef MessageResponsePtr<SC2APIProtocol::Score> ScorePtr;
typedef MessageResponsePtr<SC2APIProtocol::ScoreDetails> ScoreDestailsPtr;

bool Convert(const SC2APIProtocol::VitalScoreDetails& details_proto, VitalScoreDetails& vital_score_details) {
    vital_score_details.life = details_proto.life();
    vital_score_details.shields = details_proto.shields();
    vital_score_details.energy = details_proto.energy();
    return true;
}

bool Convert(const SC2APIProtocol::CategoryScoreDetails& details_proto, CategoryScoreDetails& category_score_details) {
    category_score_details.none = details_proto.none();
    category_score_details.army = details_proto.army();
    category_score_details.economy = details_proto.economy();
    category_score_details.technology = details_proto.technology();
    category_score_details.upgrade = details_proto.upgrade();
    return true;
}

bool Convert(const ScorePtr& score_ptr, ScoreDetails& score_details) {
    if (score_ptr.HasErrors()) {
        return false;
    }

    ScoreDestailsPtr score_details_ptr;
    SET_SUBMESSAGE_RESPONSE(score_details_ptr, score_ptr, score_details);
    if (score_details_ptr.HasErrors()) {
        return false;
    }

    score_details.idle_production_time = score_details_ptr->idle_production_time();
    score_details.idle_worker_time = score_details_ptr->idle_worker_time();
    score_details.total_value_units = score_details_ptr->total_value_units();
    score_details.total_value_structures = score_details_ptr->total_value_structures();
    score_details.killed_value_units = score_details_ptr->killed_value_units();
    score_details.killed_value_structures = score_details_ptr->killed_value_structures();
    score_details.collected_minerals = score_details_ptr->collected_minerals();
    score_details.collection_rate_minerals = score_details_ptr->collection_rate_minerals();
    score_details.collection_rate_vespene = score_details_ptr->collection_rate_vespene();
    score_details.spent_minerals = score_details_ptr->spent_minerals();
    score_details.spent_vespene = score_details_ptr->spent_vespene();

    if (score_details_ptr->has_food_used()) {
        Convert(score_details_ptr->food_used(), score_details.food_used);
    }

    if (score_details_ptr->has_killed_minerals()) {
        Convert(score_details_ptr->killed_minerals(), score_details.killed_minerals);
    }

    if (score_details_ptr->has_killed_vespene()) {
        Convert(score_details_ptr->killed_vespene(), score_details.killed_vespene);
    }

    if (score_details_ptr->has_lost_minerals()) {
        Convert(score_details_ptr->lost_minerals(), score_details.lost_minerals);
    }

    if (score_details_ptr->has_lost_vespene()) {
        Convert(score_details_ptr->lost_vespene(), score_details.lost_vespene);
    }

    if (score_details_ptr->has_friendly_fire_minerals()) {
        Convert(score_details_ptr->friendly_fire_minerals(), score_details.friendly_fire_minerals);
    }

    if (score_details_ptr->has_friendly_fire_vespene()) {
        Convert(score_details_ptr->friendly_fire_vespene(), score_details.friendly_fire_vespene);
    }

    if (score_details_ptr->has_used_minerals()) {
        Convert(score_details_ptr->used_minerals(), score_details.used_minerals);
    }

    if (score_details_ptr->has_used_vespene()) {
        Convert(score_details_ptr->used_vespene(), score_details.used_vespene);
    }

    if (score_details_ptr->has_total_used_minerals()) {
        Convert(score_details_ptr->total_used_minerals(), score_details.total_used_minerals);
    }

    if (score_details_ptr->has_total_used_vespene()) {
        Convert(score_details_ptr->total_used_vespene(), score_details.total_used_vespene);
    }

    if (score_details_ptr->has_total_damage_dealt()) {
        Convert(score_details_ptr->total_damage_dealt(), score_details.total_damage_dealt);
    }

    if (score_details_ptr->has_collected_minerals()) {
        Convert(score_details_ptr->total_damage_taken(), score_details.total_damage_taken);
    }

    if (score_details_ptr->has_total_healed()) {
        Convert(score_details_ptr->total_healed(), score_details.total_healed);
    }

    return true;
}

bool Convert(const ObservationPtr& observation_ptr, Score& score) {
    score = Score();
    if (observation_ptr.HasErrors()) {
        return false;
    }

    ScorePtr score_ptr;
    SET_SUBMESSAGE_RESPONSE(score_ptr, observation_ptr, score);
    if (score_ptr.HasErrors()) {
        return false;
    }

    score.score_type = score_ptr->has_score_type() ? ScoreType(score_ptr->score_type()) : ScoreType::Melee;
    int32_t score_i = score_ptr->score();
    score.score = float(score_i);

    Convert(score_ptr, score.score_details);

    return true;
}

bool Convert(const SC2APIProtocol::DisplayType& type_proto, Unit::DisplayType& type) {
    switch (type_proto) {
        case SC2APIProtocol::DisplayType::Visible:   type = Unit::Visible; return true;
        case SC2APIProtocol::DisplayType::Snapshot:  type = Unit::Snapshot; return true;
        case SC2APIProtocol::DisplayType::Hidden:    type = Unit::Hidden; return true;
    }
    return false;
}

bool Convert(const SC2APIProtocol::Alliance& alliance_proto, Unit::Alliance& alliance) {
    switch (alliance_proto) {
        case SC2APIProtocol::Alliance::Self:     alliance = Unit::Self; return true;
        case SC2APIProtocol::Alliance::Ally:     alliance = Unit::Ally; return true;
        case SC2APIProtocol::Alliance::Neutral:  alliance = Unit::Neutral; return true;
        case SC2APIProtocol::Alliance::Enemy:    alliance = Unit::Enemy; return true;
    }
    return false;
}

bool Convert(const SC2APIProtocol::CloakState& cloak_proto, Unit::CloakState& cloak) {
    switch (cloak_proto) {
        case SC2APIProtocol::CloakState::Cloaked:         cloak = Unit::Cloaked; return true;
        case SC2APIProtocol::CloakState::CloakedDetected: cloak = Unit::CloakedDetected; return true;
        case SC2APIProtocol::CloakState::NotCloaked:      cloak = Unit::NotCloaked; return true;
    }
    return false;
}

bool Convert(const ObservationRawPtr& observation_raw, Units& units) {
    units.clear();

    for (int i = 0; i < observation_raw->units_size(); ++i) {
        Unit unit;
        const SC2APIProtocol::Unit& observation_unit = observation_raw->units(i);

        if (!Convert(observation_unit.display_type(), unit.display_type)) {
            return false;
        }
        if (!Convert(observation_unit.alliance(), unit.alliance)) {
            return false;
        }

        unit.tag = observation_unit.tag();
        unit.unit_type = UnitTypeID(observation_unit.unit_type());
        unit.owner = observation_unit.owner();

        const SC2APIProtocol::Point& pt = observation_unit.pos();
        unit.pos.x = pt.x();
        unit.pos.y = pt.y();
        unit.pos.z = pt.z();
        unit.facing = observation_unit.facing();
        unit.radius = observation_unit.radius();
        unit.build_progress = observation_unit.build_progress();
        if (observation_unit.has_cloak()) {
            if (!Convert(observation_unit.cloak(), unit.cloak)) {
                return false;
            }
        }
        else {
            unit.cloak = Unit::Unknown;
        }

        unit.detect_range = observation_unit.detect_range();
        unit.radar_range = observation_unit.radar_range();

        unit.is_selected = observation_unit.is_selected();
        unit.is_on_screen = observation_unit.is_on_screen();
        unit.is_blip = observation_unit.is_blip();

        unit.health = observation_unit.health();
        unit.health_max = observation_unit.health_max();
        unit.shield = observation_unit.shield();
        unit.energy = observation_unit.energy();

        unit.mineral_contents = observation_unit.mineral_contents();
        unit.vespene_contents = observation_unit.vespene_contents();
        unit.is_flying = observation_unit.is_flying();
        unit.is_burrowed = observation_unit.is_burrowed();
        unit.weapon_cooldown = observation_unit.weapon_cooldown();
        unit.engaged_target_tag = observation_unit.engaged_target_tag();

        unit.orders.clear();
        for (int order_index = 0; order_index < observation_unit.orders_size(); ++order_index) {
            const SC2APIProtocol::UnitOrder& order_proto = observation_unit.orders(order_index);

            UnitOrder order;
            order.ability_id = order_proto.ability_id();
            order.target_unit_tag = order_proto.target_unit_tag();
            order.target_pos.x = order_proto.target_world_space_pos().x();
            order.target_pos.y = order_proto.target_world_space_pos().y();
            order.progress = order_proto.progress();
            unit.orders.push_back(order);
        }

        unit.add_on_tag = observation_unit.add_on_tag();

        unit.passengers.clear();
        for (int passenger_index = 0; passenger_index < observation_unit.passengers_size(); ++passenger_index) {
            const SC2APIProtocol::PassengerUnit& passengerProto = observation_unit.passengers(passenger_index);
            PassengerUnit passengerUnit;
            if (passengerProto.has_tag())
                passengerUnit.tag = passengerProto.tag();
            if (passengerProto.has_health())
                passengerUnit.health = passengerProto.health();
            if (passengerProto.has_health_max())
                passengerUnit.health_max = passengerProto.health_max();
            if (passengerProto.has_shield())
                passengerUnit.shield = passengerProto.shield();
            if (passengerProto.has_energy())
                passengerUnit.energy = passengerProto.energy();
            if (passengerProto.has_unit_type())
                passengerUnit.unit_type = passengerProto.unit_type();
            unit.passengers.push_back(passengerUnit);
        }

        unit.cargo_space_taken= observation_unit.cargo_space_taken();
        unit.cargo_space_max = observation_unit.cargo_space_max();
        unit.assigned_harvesters = observation_unit.assigned_harvesters();
        unit.ideal_harvesters = observation_unit.ideal_harvesters();

        for (int buff_index = 0; buff_index < observation_unit.buff_ids_size(); ++buff_index) {
            unit.buffs.push_back(observation_unit.buff_ids(buff_index));
        }

        units.push_back(unit);
    }

    return units.size() > 0;
}

bool Convert(const SC2APIProtocol::ImageData& image, ImageData& data) {
    data.width = image.size().x();
    data.height = image.size().y();
    data.bits_per_pixel = image.bits_per_pixel();
    data.data = image.data();

    int expectedSizeBits = data.width * data.height * data.bits_per_pixel;
    return expectedSizeBits > 0 && data.data.size() * 8 == expectedSizeBits;
}

bool Convert(const ObservationPtr& observation_ptr, RenderedFrame& render) {
    ObservationRenderPtr observation_render;
    SET_SUBMESSAGE_RESPONSE(observation_render, observation_ptr, render_data);
    if (observation_render.HasErrors()) {
        return false;
    }

    if (!observation_render->has_map()) {
        return false;
    }

    if (!Convert(observation_render->map(), render.map)) {
        return false;
    }

    if (!observation_render->has_minimap()) {
        return false;
    }

    if (!Convert(observation_render->minimap(), render.minimap)) {
        return false;
    }

    return true;
}

bool Convert(const ResponseObservationPtr& response_observation_ptr, RawActions& actions, const Units& /*units*/, uint32_t /*player_id*/) {
    for (int i = 0; i < response_observation_ptr->actions_size(); ++i) {
        const SC2APIProtocol::Action& proto_action = response_observation_ptr->actions(i);
        if (!proto_action.has_action_raw()) {
            continue;
        }
        const SC2APIProtocol::ActionRaw& action_raw = proto_action.action_raw();
        if (!action_raw.has_unit_command()) {
            continue;
        }
        const SC2APIProtocol::ActionRawUnitCommand& action_raw_command = action_raw.unit_command();
        if (!action_raw_command.has_ability_id()) {
            continue;
        }

        // Construct and push the relevant action.
        ActionRaw action;
        action.ability_id = AbilityID(action_raw_command.ability_id());

        if (action_raw_command.has_target_unit_tag()) {
            action.target_type = ActionRaw::TargetUnitTag;
            action.target_tag = action_raw_command.target_unit_tag();
        }
        else if (action_raw_command.has_target_world_space_pos()) {
            action.target_type = ActionRaw::TargetPosition;
            action.target_point.x = action_raw_command.target_world_space_pos().x();
            action.target_point.y = action_raw_command.target_world_space_pos().y();
        }

        for (int j = 0; j < action_raw_command.unit_tags_size(); ++j)
            action.unit_tags.push_back(action_raw_command.unit_tags(j));

        // TODO: Add optional target positions.
        // optional Point target_world_space_pos = 2;

        actions.push_back(action);
    }

    return true;
}

bool Convert(const SC2APIProtocol::ActionSpatialUnitSelectionPoint::Type& type_proto, PointSelectionType& type) {
    switch (type_proto) {
        case SC2APIProtocol::ActionSpatialUnitSelectionPoint::Select:       type = PointSelectionType::PtSelect; return true;
        case SC2APIProtocol::ActionSpatialUnitSelectionPoint::Toggle:       type = PointSelectionType::PtToggle; return true;
        case SC2APIProtocol::ActionSpatialUnitSelectionPoint::AllType:      type = PointSelectionType::PtAllType; return true;
        case SC2APIProtocol::ActionSpatialUnitSelectionPoint::AddAllType:   type = PointSelectionType::PtAddAllType; return true;
    }
    return false;
}

bool Convert(const ResponseObservationPtr& response_observation_ptr, SpatialActions& actions, const Units& /*units*/, uint32_t /*player_id*/) {
    for (int i = 0; i < response_observation_ptr->actions_size(); ++i) {
        const SC2APIProtocol::Action& proto_action = response_observation_ptr->actions(i);
        if (!proto_action.has_action_feature_layer()) {
            continue;
        }
        const SC2APIProtocol::ActionSpatial& action_FL = proto_action.action_feature_layer();

        if (action_FL.has_unit_command()) {
            const SC2APIProtocol::ActionSpatialUnitCommand& action_command = action_FL.unit_command();

            SpatialUnitCommand command;
            command.ability_id = AbilityID(action_command.ability_id());
            if (action_command.has_target_screen_coord()) {
                command.target_type = SpatialUnitCommand::TargetScreen;
                command.target.x = action_command.target_screen_coord().x();
                command.target.y = action_command.target_screen_coord().y();
            }
            else if (action_command.has_target_minimap_coord()) {
                command.target_type = SpatialUnitCommand::TargetMinimap;
                command.target.x = action_command.target_minimap_coord().x();
                command.target.y = action_command.target_minimap_coord().y();
            }
            command.queued = action_command.queue_command();

            actions.unit_commands.push_back(command);
        }
        else if (action_FL.has_camera_move()) {
            const SC2APIProtocol::ActionSpatialCameraMove& action_camera = action_FL.camera_move();

            SpatialCameraMove camera;
            camera.center_minimap.x = action_camera.center_minimap().x();
            camera.center_minimap.y = action_camera.center_minimap().y();

            actions.camera_moves.push_back(camera);
        }
        else if (action_FL.has_unit_selection_point()) {
            const SC2APIProtocol::ActionSpatialUnitSelectionPoint& action_select = action_FL.unit_selection_point();

            SpatialSelectPoint select;
            select.select_screen.x = action_select.selection_screen_coord().x();
            select.select_screen.y = action_select.selection_screen_coord().y();
            if (!Convert(action_select.type(), select.type))
                continue;

            actions.select_points.push_back(select);
        }
        else if (action_FL.has_unit_selection_rect()) {
            const SC2APIProtocol::ActionSpatialUnitSelectionRect& action_select = action_FL.unit_selection_rect();

            SpatialSelectRect select;
            for (int j = 0; j < action_select.selection_screen_coord_size(); ++j) {
                const SC2APIProtocol::RectangleI& rect_proto = action_select.selection_screen_coord(j);

                Rect2DI rect;
                rect.from.x = rect_proto.p0().x();
                rect.from.y = rect_proto.p0().y();
                rect.to.x = rect_proto.p1().x();
                rect.to.y = rect_proto.p1().y();
                select.select_screen.push_back(rect);
            }
            select.select_add = action_select.selection_add();

            actions.select_rects.push_back(select);
        }
    }

    return true;
}

void Convert(const SC2APIProtocol::SpatialCameraSetup& setup_proto, SpatialSetup& setup) {
    setup.camera_width = setup_proto.width();
    const SC2APIProtocol::Size2DI& resolution = setup_proto.resolution();
    const SC2APIProtocol::Size2DI& minimap_resolution = setup_proto.minimap_resolution();
    setup.map_resolution_x= resolution.x();
    setup.map_resolution_y= resolution.y();
    setup.minimap_resolution_x= minimap_resolution.x();
    setup.minimap_resolution_y= minimap_resolution.y();
}

bool Convert(const SC2APIProtocol::InterfaceOptions& options_proto, InterfaceOptions& options) {
    if (!options_proto.has_raw()) {
        return false;
    }
    options.raw = options_proto.raw();

    Convert(options_proto.feature_layer(), options.feature_layer);
    Convert(options_proto.render(), options.render);

    return true;
}

bool Convert(const ResponseGameInfoPtr& response_game_info_ptr, GameInfo& game_info) {
    if (!response_game_info_ptr->has_start_raw()) {
        return false;
    }
    const SC2APIProtocol::StartRaw& start_raw = response_game_info_ptr->start_raw();

    if (!start_raw.has_map_size() || !start_raw.map_size().has_x() || !start_raw.map_size().has_y()) {
        return false;
    }
    game_info.width = static_cast<int>(start_raw.map_size().x());
    game_info.height = static_cast<int>(start_raw.map_size().y());

    if (start_raw.has_pathing_grid()) {
        Convert(start_raw.pathing_grid(), game_info.pathing_grid);
    }

    if (start_raw.has_terrain_height()) {
        Convert(start_raw.terrain_height(), game_info.terrain_height);
    }

    if (start_raw.has_placement_grid()) {
        Convert(start_raw.placement_grid(), game_info.placement_grid);
    }

    if (start_raw.has_playable_area()) {
        if (start_raw.playable_area().has_p0()) {
            game_info.playable_min.x = static_cast<float>(start_raw.playable_area().p0().x());
            game_info.playable_min.y = static_cast<float>(start_raw.playable_area().p0().y());
        }

        if (start_raw.playable_area().has_p1()) {
            game_info.playable_max.x = static_cast<float>(start_raw.playable_area().p1().x());
            game_info.playable_max.y = static_cast<float>(start_raw.playable_area().p1().y());
        }
    }

    game_info.enemy_start_locations.clear();
    for (int i = 0, e = start_raw.start_locations_size(); i < e; ++i) {
        const SC2APIProtocol::Point2D& pt = start_raw.start_locations(i);
        game_info.enemy_start_locations.push_back(sc2::Point2D(pt.x(), pt.y()));
    }

    for (const auto& player_info : response_game_info_ptr->player_info()) {
        game_info.player_info.push_back(sc2::PlayerInfo(
            static_cast<uint32_t>(player_info.player_id()),
            ConvertPlayerTypeFromProto(player_info.type()),
            ConvertRaceFromProto(player_info.race_requested()),
            ConvertRaceFromProto(player_info.race_actual()),
            ConvertDifficultyFromProto(player_info.difficulty())
        ));
    }

    if (!response_game_info_ptr->has_options()) {
        return false;
    }

    if (!Convert(response_game_info_ptr->options(), game_info.options)) {
        return false;
    }

    return true;
}

Race ConvertRaceFromProto(SC2APIProtocol::Race race) {
    switch (race) {
        case SC2APIProtocol::Terran: {
            return Terran;
        }
        case SC2APIProtocol::Zerg: {
            return Zerg;
        }
        case SC2APIProtocol::Protoss: {
            return Protoss;
        }
        default: {
            break;
        }
    }
    return Random;
}

GameResult ConvertGameResultFromProto(SC2APIProtocol::Result result) {
    switch (result) {
        case SC2APIProtocol::Victory: {
            return Win;
        }
        case SC2APIProtocol::Defeat: {
            return Loss;
        }
        case SC2APIProtocol::Tie: {
            return Tie;
        }
        default: {
            break;
        }
    }
    return Undecided;
}

PlayerType ConvertPlayerTypeFromProto(SC2APIProtocol::PlayerType type) {
    switch (type) {
        case SC2APIProtocol::Participant: {
            return Participant;
        }
        case SC2APIProtocol::Computer: {
            return Computer;
        }
        case SC2APIProtocol::Observer: {
            return Observer;
        }
    }
    return Observer;
}

Difficulty ConvertDifficultyFromProto(SC2APIProtocol::Difficulty difficulty) {
    switch (difficulty) {
        case SC2APIProtocol::VeryEasy: {
            return VeryEasy;
        }
        case SC2APIProtocol::Easy: {
            return Easy;
        }
        case SC2APIProtocol::Medium: {
            return Medium;
        }
        case SC2APIProtocol::MediumHard: {
            return MediumHard;
        }
        case SC2APIProtocol::Hard: {
            return Hard;
        }
        case SC2APIProtocol::Harder: {
            return HardVeryHard;
        }
        case SC2APIProtocol::VeryHard: {
            return VeryHard;
        }
        case SC2APIProtocol::CheatVision: {
            return CheatVision;
        }
        case SC2APIProtocol::CheatMoney: {
            return CheatMoney;
        }
        case SC2APIProtocol::CheatInsane: {
            return CheatInsane;
        }
    }
    return VeryEasy;
}

}
