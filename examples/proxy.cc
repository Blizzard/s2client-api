// This example is a proxy example for forwarding messages from a bot to sc2.
// It requires the coordinator to be in an attach mode instead of launching the game itself.

#include "sc2api/sc2_server.h"
#include "sc2api/sc2_connection.h"
#include "sc2api/sc2_args.h"

#include "sc2utils/sc2_manage_process.h"
#include "s2clientprotocol/sc2api.pb.h"
#include "sc2api/sc2_proto_interface.h"
#include "sc2api/sc2_client.h"

#include <iostream>

using namespace sc2;

sc2::GameRequestPtr CreateStartGameRequest(std::string MapName, std::vector<sc2::PlayerSetup> players)
{
    sc2::ProtoInterface proto;
    sc2::GameRequestPtr request = proto.MakeRequest();
    SC2APIProtocol::RequestCreateGame* request_create_game = request->mutable_create_game();
    for (const sc2::PlayerSetup& setup : players)
    {
        SC2APIProtocol::PlayerSetup* playerSetup = request_create_game->add_player_setup();
        playerSetup->set_type(SC2APIProtocol::PlayerType(setup.type));
        playerSetup->set_race(SC2APIProtocol::Race(int(setup.race) + 1));
        playerSetup->set_difficulty(SC2APIProtocol::Difficulty(setup.difficulty));
    }

    request_create_game->set_realtime(false);
    return request;
}

void GameUpdate(sc2::Connection *Client, sc2::Server *Server)
{
    while (!sc2::PollKeyPress()) {
        // If the proxy has messages forward them to sc2.
        if (Server->HasRequest()) {
            Server->SendRequest(Client->connection_);

            // Block for sc2's response then queue it.
            SC2APIProtocol::Response* response = nullptr;
            Client->Receive(response, 100000);
            Server->QueueResponse(Client->connection_, response);

            std::cout << "Sending response" << std::endl;

            // Send the response back to the client.
            Server->SendResponse();
        }
    }
}

int main(int argc, char* argv[]) {
    // Setup server that mimicks sc2.
    sc2::Server server;
    sc2::Server server2;
    server.Listen("5678", "100000", "100000", "5");
    server2.Listen("5677", "100000", "100000", "5");

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

    sc2::StartProcess(process_settings.process_path,
    { "-listen", "127.0.0.1",
        "-port", "5680",
        "-displayMode", "0",
        "-dataVersion", process_settings.data_version }
    );
    sc2::SleepFor(10000);

    // Connect to running sc2 process.
    sc2::Connection client;
    client.Connect("127.0.0.1", 5679);
    sc2::Connection client2;
    client2.Connect("127.0.0.1", 5680);
    std::vector<PlayerSetup> Players;
    Players.push_back(PlayerSetup(PlayerType::Participant, Race::Terran, nullptr, sc2::Easy));
    Players.push_back(PlayerSetup(PlayerType::Participant, Race::Terran, nullptr, sc2::Easy));
    sc2::GameRequestPtr Create_game_request = CreateStartGameRequest("AscensiontoAiurLE.SC2Map", Players);
    client.Send(Create_game_request.get());
    SC2APIProtocol::Response* create_response = nullptr;
    if (client.Receive(create_response, 100000))
    {
        std::cout << "Recieved create game response " << create_response->data().SerializeAsString() << std::endl;
    }
    std::thread bot1Thread(&GameUpdate, &client, &server);
    std::thread bot2Thread(&GameUpdate, &client2, &server2);
    bot1Thread.join();
    bot2Thread.join();


    return 0;
}
