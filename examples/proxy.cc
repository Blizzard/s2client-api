// This example is a proxy example for forwarding messages from a bot to sc2.
// It requires the coordinator to be in an attach mode instead of launching the game itself.

#include "sc2api/sc2_server.h"
#include "sc2api/sc2_connection.h"
#include "sc2api/sc2_args.h"

#include "sc2utils/sc2_manage_process.h"
#include "s2clientprotocol/sc2api.pb.h"

#include <iostream>

int main(int argc, char* argv[]) {
    // Setup server that mimicks sc2.
    sc2::Server server;
    server.Listen("5678", "100000", "100000", "5");

    // Find game executable and run it.
    sc2::ProcessSettings process_settings;
    sc2::GameSettings game_settings;
    sc2::ParseSettings(argc, argv, process_settings, game_settings);
    sc2::StartProcess(process_settings.process_path,
        { "-listen", "127.0.0.1",
          "-port", "5679",
          "-displayMode", "0",
          "-dataVersion", process_settings.data_version }
    );

    sc2::SleepFor(10000);

    // Connect to running sc2 process.
    sc2::Connection client;
    client.Connect("127.0.0.1", 5679);

    while (!sc2::PollKeyPress()) {
        // If the proxy has messages forward them to sc2.
        if (server.HasRequest()) {
            server.SendRequest(client.connection_);

            // Block for sc2's response then queue it.
            SC2APIProtocol::Response* response = nullptr;
            client.Receive(response, 100000);
            server.QueueResponse(client.connection_, response);

            std::cout << "Sending response" << std::endl;

            // Send the response back to the client.
            server.SendResponse();
        }
    }

    return 0;
}
