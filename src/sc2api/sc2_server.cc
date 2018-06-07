#include "sc2api/sc2_server.h"

#include "s2clientprotocol/sc2api.pb.h"

#include "civetweb.h"

#include <cstring>
#include <iostream>

namespace sc2 {

bool VERBOSE = false;

bool GetServerData(const mg_connection* conn, void* data, Server*& out) {
    if (!conn) {
        return false;
    }

    if (!data) {
        return false;
    }

    out = (Server*)data;
    return true;
}

static int WebSocketConnectHandler(const mg_connection* conn, void* websocket_server) {
    if (VERBOSE) std::cout << "Client connected (" << conn << ")" << std::endl;
    Server* server;
    if (!GetServerData(conn, websocket_server, server)) {
        return 0;
    }

    server->connections_.push_back(conn);

    return 0;
}

static int WebSocketDataHandler(
    struct mg_connection* conn,
    int /*flags*/,
    char* data,
    size_t len,
    void* websocket_server) {
    Server* server;
    if (!GetServerData(conn, websocket_server, server)) {
        return 0;
    }

    if (VERBOSE) std::cout << "Client data (" << conn << ")" << std::endl;

    SC2APIProtocol::Request* request = new SC2APIProtocol::Request();
    if (!request->ParseFromArray(data, static_cast<int>(len))) {
        // The server can only receive valid requests off civetweb threads. Otherwise, die.
        return 0;
    }

    server->QueueRequest(conn, request);
    
    return 1;
}

static void WebSocketCloseHandler(const mg_connection* conn, void* websocket_server) {
    sc2::Server* server;
    if (!GetServerData(conn, websocket_server, server)) {
        return;
    }

    // Convert to blz::erase_if
    for (size_t i = 0; i < server->connections_.size(); ++i) {
        mg_connection* target = (mg_connection*)server->connections_[i];
        if (target == conn) {
            server->connections_.erase(server->connections_.begin() + i);
            break;
        }
    }
}

template<class T>
static void SendMessage(mg_connection* conn, std::queue<T>& message_queue) {
    if (message_queue.empty()) {
        return;
    }

    if (VERBOSE) std::cout << "SendMessage (" << conn << ")" << std::endl;

    google::protobuf::Message* message = message_queue.front().second;
    size_t size = message->ByteSize();
    char* bytes = new char[size];
    message->SerializeToArray(bytes, (int)size);
    mg_websocket_write(
        conn,
        MG_WEBSOCKET_OPCODE_BINARY,
        (const char*)bytes,
        size
    );
    message_queue.pop();
    delete [] bytes;
    delete message;
}

Server::Server() {
}

Server::~Server() {
    mg_stop(mg_context_);
}

bool Server::Listen(
    const char* listeningPorts,
    const char* requestTimeoutMs,
    const char* websocketTimeoutMs,
    const char* numThreads) {
    const char *options[] = {
        "listening_ports",
        listeningPorts,
        "request_timeout_ms",
        requestTimeoutMs,
        "websocket_timeout_ms",
        websocketTimeoutMs,
        "num_threads",
        numThreads,
        "tcp_nodelay",
        "1",
        0
    };

    mg_callbacks callbacks;
    memset(&callbacks, 0, sizeof(callbacks));
    mg_context_ = mg_start(&callbacks, nullptr, options);

    if (!mg_context_) {
        return false;
    }

    mg_set_websocket_handler(
        mg_context_,
        "/sc2api",
        WebSocketConnectHandler,
        nullptr,
        WebSocketDataHandler,
        WebSocketCloseHandler,
        (void*) this
    );

    return true;
}

void Server::QueueRequest(struct mg_connection* conn, SC2APIProtocol::Request*& request) {
    request_mutex_.lock();
    requests_.push(RequestData(conn, request));
    request_mutex_.unlock();
}

void Server::QueueResponse(struct mg_connection* conn, SC2APIProtocol::Response*& response) {
    response_mutex_.lock();
    responses_.push(ResponseData(conn, response));
    response_mutex_.unlock();
}

void Server::SendRequest(struct mg_connection* conn) {
    request_mutex_.lock();
    SendMessage(conn ? conn : (mg_connection*)connections_.front(), requests_);
    request_mutex_.unlock();
}

void Server::SendResponse(struct mg_connection* conn) {
    response_mutex_.lock();
    SendMessage(conn ? conn : (mg_connection*)connections_.front(), responses_);
    response_mutex_.unlock();
}

bool Server::HasRequest() {
    request_mutex_.lock();
    bool empty = requests_.empty();
    request_mutex_.unlock();
    return !empty;
}

bool Server::HasResponse() {
    response_mutex_.lock();
    bool empty = responses_.empty();
    response_mutex_.unlock();
    return !empty;
}

const RequestData& Server::PeekRequest() {
    return requests_.front();
}

const ResponseData& Server::PeekResponse() {
    return responses_.front();
}

}
