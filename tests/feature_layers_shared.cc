#include "test_framework.h"
#include "test_movement_combat.h"
#include "sc2api/sc2_api.h"
#include <iostream>
#include <string>
#include <random>
#include "feature_layers_shared.h"

namespace sc2 {

const char* GetPlayerRelativeLayer(Agent* agent, FeatureLayerType type, FeatureLayer8BPP& layer) {
    const SC2APIProtocol::Observation* observation = agent->Observation()->GetRawObservation();
    if (!observation->has_feature_layer_data())
        return "No feature layer data";

    const SC2APIProtocol::ObservationFeatureLayer& observationFL = observation->feature_layer_data();

    const SC2APIProtocol::ImageData* image = nullptr;

    if (type == FeatureLayerType::MAP) {
        if (!observationFL.has_renders())
            return "No map data";

        const SC2APIProtocol::FeatureLayers& map = observationFL.renders();
        if (!map.has_player_relative())
            return "No player_relative layer in map";

        layer.width = map.player_relative().size().x();
        layer.height = map.player_relative().size().y();
        image = &map.player_relative();
    }
    else {
        if (!observationFL.has_minimap_renders())
            return "No minimap data";

        const SC2APIProtocol::FeatureLayersMinimap& minimap = observationFL.minimap_renders();
        if (!minimap.has_player_relative())
            return "No player_relative layer in minimap";
        
        layer.width = minimap.player_relative().size().x();
        layer.height = minimap.player_relative().size().y();
        image = &minimap.player_relative();
    }
    
    if (image == nullptr)
        return "No image data";

    int bitsPerPixel = image->bits_per_pixel();
    if (bitsPerPixel != 8)
        return "bits_per_pixel of player_relative incorrect size";

    const std::string& data = image->data();
    if (data.size() != layer.width * layer.height)
        return "Feature layer data incorrect size";

    layer.data = data;
    return nullptr;
}

Point2DI ConvertWorldToMinimap(const GameInfo& game_info, const Point2D& world) {
    int image_width = game_info.options.feature_layer.minimap_resolution_x;
    int image_height = game_info.options.feature_layer.minimap_resolution_y;
    float map_width = (float)game_info.width;
    float map_height = (float)game_info.height;

    // Pixels always cover a square amount of world space. The scale is determined
    // by the largest axis of the map.
    float pixel_size = std::max(map_width / image_width, map_height / image_height);

    // Origin of world space is bottom left. Origin of image space is top left.
    // Upper left corner of the map corresponds to the upper left corner of the upper 
    // left pixel of the feature layer.
    float image_origin_x = 0;
    float image_origin_y = map_height;
    float image_relative_x = world.x - image_origin_x;
    float image_relative_y = image_origin_y - world.y;

    int image_x = static_cast<int>((image_relative_x / pixel_size));
    int image_y = static_cast<int>((image_relative_y / pixel_size));

    return Point2DI(image_x, image_y);
}

Point2DI ConvertWorldToCamera(const GameInfo& game_info, const Point2D camera_world, const Point2D& world) {
    float camera_size = game_info.options.feature_layer.camera_width;
    int image_width = game_info.options.feature_layer.map_resolution_x;
    int image_height = game_info.options.feature_layer.map_resolution_y;

    // Pixels always cover a square amount of world space. The scale is determined
    // by making the shortest axis of the camera match the requested camera_size.
    float pixel_size = camera_size / std::min(image_width, image_height);
    float image_width_world = pixel_size * image_width;
    float image_height_world = pixel_size * image_height;

    // Origin of world space is bottom left. Origin of image space is top left.
    // The feature layer is centered around the camera target position.
    float image_origin_x = camera_world.x - image_width_world / 2.0f;
    float image_origin_y = camera_world.y + image_height_world / 2.0f;
    float image_relative_x = world.x - image_origin_x;
    float image_relative_y = image_origin_y - world.y;

    int image_x = static_cast<int>(image_relative_x / pixel_size);
    int image_y = static_cast<int>(image_relative_y / pixel_size);

    return Point2DI(image_x, image_y);
}

}

