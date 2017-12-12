#include "sc2api/sc2_api.h"
#include "sc2utils/sc2_manage_process.h"
#include "sc2renderer/sc2_renderer.h"

#include <iostream>

#define LINUX_USE_SOFTWARE_RENDER 0

const int kMapX = 800;
const int kMapY = 600;
const int kMiniMapX = 300;
const int kMiniMapY = 300;

class RenderAgent : public sc2::Agent {
public:
    virtual void OnGameStart() final {
        sc2::renderer::Initialize("Rendered", 50, 50, kMiniMapX + kMapX, std::max(kMiniMapY, kMapY));
    }

    virtual void OnStep() final {
        const SC2APIProtocol::Observation* observation = Observation()->GetRawObservation();
        const SC2APIProtocol::ObservationRender& render =  observation->render_data();

        const SC2APIProtocol::ImageData& minimap = render.minimap();
        sc2::renderer::ImageRGB(&minimap.data().data()[0], minimap.size().x(), minimap.size().y(), 0, std::max(kMiniMapY, kMapY) - kMiniMapY);

        const SC2APIProtocol::ImageData& map = render.map();
        sc2::renderer::ImageRGB(&map.data().data()[0], map.size().x(), map.size().y(), kMiniMapX, 0);

        sc2::renderer::Render();
    }

    virtual void OnGameEnd() final {
        sc2::renderer::Shutdown();
    }
};

int main(int argc, char* argv[]) {
    sc2::Coordinator coordinator;
    if (!coordinator.LoadSettings(argc, argv)) {
        std::cout << "Unable to find or parse settings." << std::endl;
        return 1;
    }

    sc2::RenderSettings settings(kMapX, kMapY, kMiniMapX, kMiniMapY);
    coordinator.SetRender(settings);

#if defined(__linux__)
#if LINUX_USE_SOFTWARE_RENDER
    coordinator.AddCommandLine("-osmesapath /usr/lib/x86_64-linux-gnu/libOSMesa.so");
#else
    coordinator.AddCommandLine("-eglpath libEGL.so");
#endif
#endif

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
