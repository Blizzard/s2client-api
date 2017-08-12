#include "sc2api/sc2_api.h"
#include "sc2utils/sc2_manage_process.h"

#include <iostream>

#include "sc2renderer/sc2_renderer.h"

const float kCameraWidth = 24.0f;
const int kFeatureLayerSize = 80;
const int kPixelDrawSize = 5;
const int kDrawSize = kFeatureLayerSize * kPixelDrawSize;

void DrawFeatureLayer1BPP(const SC2APIProtocol::ImageData& image_data, int off_x, int off_y) {
    assert(image_data.bits_per_pixel() == 1);
    int width = image_data.size().x();
    int height = image_data.size().y();
    sc2::renderer::Matrix1BPP(image_data.data().c_str(), width, height, off_x, off_y, kPixelDrawSize, kPixelDrawSize);
}

void DrawFeatureLayerUnits8BPP(const SC2APIProtocol::ImageData& image_data, int off_x, int off_y) {
    assert(image_data.bits_per_pixel() == 8);
    int width = image_data.size().x();
    int height = image_data.size().y();
    sc2::renderer::Matrix8BPPPlayers(image_data.data().c_str(), width, height, off_x, off_y, kPixelDrawSize, kPixelDrawSize);
}

void DrawFeatureLayerHeightMap8BPP(const SC2APIProtocol::ImageData& image_data, int off_x, int off_y) {
    assert(image_data.bits_per_pixel() == 8);
    int width = image_data.size().x();
    int height = image_data.size().y();
    sc2::renderer::Matrix8BPPHeightMap(image_data.data().c_str(), width, height, off_x, off_y, kPixelDrawSize, kPixelDrawSize);
}

class RenderAgent : public sc2::Agent {
public:
    virtual void OnGameStart() final {
        sc2::renderer::Initialize("Feature layers", 50, 50, 2 * kDrawSize, 2 * kDrawSize);
    }

    virtual void OnStep() final {
        const SC2APIProtocol::Observation* observation = Observation()->GetRawObservation();

        const SC2APIProtocol::FeatureLayers& m = observation->feature_layer_data().renders();
        DrawFeatureLayerUnits8BPP(m.unit_density(), 0, 0);
        DrawFeatureLayer1BPP(m.selected(), kDrawSize, 0);

        const SC2APIProtocol::FeatureLayersMinimap& mi = observation->feature_layer_data().minimap_renders();
        DrawFeatureLayerHeightMap8BPP(mi.height_map(), 0, kDrawSize);
        DrawFeatureLayer1BPP(mi.camera(), kDrawSize, kDrawSize);

        sc2::renderer::Render();
    }

    virtual void OnGameEnd() final {
        sc2::renderer::Shutdown();
    }
};

int main(int argc, char* argv[]) {
    sc2::Coordinator coordinator;
    if (!coordinator.LoadSettings(argc, argv)) {
        return 1;
    }

    sc2::FeatureLayerSettings settings(kCameraWidth, kFeatureLayerSize, kFeatureLayerSize, kFeatureLayerSize, kFeatureLayerSize);
    coordinator.SetFeatureLayers(settings);

    // Add the custom bot, it will control the players.
    RenderAgent bot;

    coordinator.SetParticipants({
        CreateParticipant(sc2::Race::Terran, &bot),
        CreateComputer(sc2::Race::Zerg)
    });

    // Start the game.
    coordinator.LaunchStarcraft();
    coordinator.StartGame(sc2::kMapBelShirVestigeLE);

    while (coordinator.Update()) {
        if (sc2::PollKeyPress()) {
            break;
        }
    }

    return 0;
}
