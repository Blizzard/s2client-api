#include "test_framework.h"
#include "sc2api/sc2_api.h"

#include <algorithm>

namespace sc2 {

const int kMapX = 800;
const int kMapY = 600;
const int kMiniMapX = 300;
const int kMiniMapY = 300;

//
// TestRenderBasic
//

class TestRenderBasic : public TestSequence {
public:

    void OnTestStart() override {
        wait_game_loops_ = 5;
    }

    void VerifyRenderedData (const SC2APIProtocol::ImageData& image) {
        if (image.bits_per_pixel() != 24) {
            ReportError("Image data not RGB format.");
            return;
        }

        const std::string& data = image.data();

        int expected_size = 3 * image.size().x() * image.size().y();
        if (data.size() != expected_size) {
            ReportError("Image data unexpected size.");
            return;
        }

        const unsigned char* buffer = (unsigned char*)&data[0];
        bool found[256] = {0};
        for (int i = 0; i < data.size(); ++i) {
            found[buffer[i]] = true;
        }

        if (std::count(found, found + 256, true) <= 1) {
            ReportError("Rendering likely failed. Data is solid color.");
            return;
        }
    }

    void OnTestFinish() override {
        const ObservationInterface* obs = agent_->Observation();

        const SpatialSetup& setup = obs->GetGameInfo().options.render;
        if (setup.map_resolution_x != kMapX
            || setup.map_resolution_y != kMapY
            || setup.minimap_resolution_x != kMiniMapX
            || setup.minimap_resolution_y != kMiniMapY
        ) {
            ReportError("Interface options unexpected size.");
            return;
        }

        const SC2APIProtocol::Observation* observation = obs->GetRawObservation();
        if (!observation->has_render_data()) {
            ReportError("Missing rendered observation.");
            return;
        }

        const SC2APIProtocol::ObservationRender& obs_render = observation->render_data();
        if (!obs_render.has_map() || !obs_render.has_minimap()) {
            ReportError("Missing rendered data.");
            return;
        }

        VerifyRenderedData(obs_render.map());
        VerifyRenderedData(obs_render.minimap());
    }
};

//
// RenderedTestBot
//

class RenderedTestBot : public UnitTestBot {
public:
    RenderedTestBot();

private:
    void OnTestsBegin () final;
    void OnTestsEnd () final;
};

RenderedTestBot::RenderedTestBot() :
    UnitTestBot() {
    // Sequences.
    Add(TestRenderBasic());
}

void RenderedTestBot::OnTestsBegin() {
};

void RenderedTestBot::OnTestsEnd () {
}


//
// TestRendered
//

bool TestRendered(int argc, char** argv) {
    Coordinator coordinator;
    if (!coordinator.LoadSettings(argc, argv)) {
        return false;
    }

    RenderSettings settings(kMapX, kMapY, kMiniMapX, kMiniMapY);
    coordinator.SetRender(settings);

#if defined(__linux__)
    coordinator.AddCommandLine("-eglpath libEGL.so");
#endif

    // Add the custom bot, it will control the players.
    RenderedTestBot bot;

    coordinator.SetParticipants({
        CreateParticipant(sc2::Race::Terran, &bot),
    });

    // Start the game.
    coordinator.LaunchStarcraft();
    coordinator.StartGame(sc2::kMapEmpty);

    // Step forward the game simulation.
    while (!bot.IsFinished()) {
        coordinator.Update();
    }

    return bot.Success();
}

}
