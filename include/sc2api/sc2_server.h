/*! \file sc2_server.h
    \brief A basic websocket server for sc2.
*/

#pragma once

#include <utility>
#include <queue>
#include <vector>
#include <mutex>

struct mg_connection;
struct mg_context;

namespace SC2APIProtocol {
    class Request;
    class Response;
}

namespace sc2 {

typedef std::pair<struct mg_connection*, SC2APIProtocol::Request*> RequestData;
typedef std::pair<struct mg_connection*, SC2APIProtocol::Response*> ResponseData;

class Server {
public:
    Server();
    ~Server();

    bool Listen(
        const char* listeningPorts,
        const char* requestTimeoutMs,
        const char* websocketTimeoutMs,
        const char* numThreads);

    void QueueRequest(struct mg_connection* conn, SC2APIProtocol::Request*& request);
    void QueueResponse(struct mg_connection* conn, SC2APIProtocol::Response*& response);

    // If no connection is provided send it to the first connection attained.
    void SendRequest(struct mg_connection* conn = nullptr);
    void SendResponse(struct mg_connection* conn = nullptr);

    bool HasRequest();
    bool HasResponse();

    const RequestData& PeekRequest();
    const ResponseData& PeekResponse();

    std::vector<const mg_connection*> connections_;
private:
    mg_context* mg_context_ = nullptr;

    std::queue<RequestData> requests_;
    std::queue<ResponseData> responses_;

    std::mutex request_mutex_;
    std::mutex response_mutex_;
};

}
