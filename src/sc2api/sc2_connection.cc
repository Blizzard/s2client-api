#include "sc2api/sc2_connection.h"

#include <iostream>
#include <cassert>
#include <chrono>

#include "s2clientprotocol/sc2api.pb.h"

#include "civetweb.h"

#if defined (_WIN32)
#include <WinSock2.h>
#endif

namespace sc2 {

void StartCivetweb() {
    static bool is_initialized = false;
    static const char* REQUEST_TIMEOUT_MS = "5000";
    static const char* WEBSOCKET_TIMEOUT_MS = "1200000";
    static const char* NUM_THREADS = "4";
    static const char* NO_DELAY = "1";

    if (is_initialized) {
        return;
    }

    const char* options[] = {
        "request_timeout_ms",
        REQUEST_TIMEOUT_MS,
        "websocket_timeout_ms",
        WEBSOCKET_TIMEOUT_MS,
        "num_threads",
        NUM_THREADS,
        "tcp_nodelay",
        NO_DELAY,
        0
    };

    mg_callbacks callbacks;
    memset(&callbacks, 0, sizeof(callbacks));
    mg_start(&callbacks, nullptr, options);

    is_initialized = true;
}

bool GetClientData(const mg_connection* connection, sc2::Connection*& out) {
    if (!connection) {
        return false;
    }

    mg_context* ctx = mg_get_context(connection);

    if (!ctx) {
        return false;
    }

    out = (sc2::Connection*) mg_get_user_data(ctx);
    return true;
}

static int DataHandler(
    mg_connection* conn,
    int /*flags*/,
    char* data,
    size_t data_len,
    void*) {
    sc2::Connection* sc2_connection;
    if (!GetClientData(conn, sc2_connection)) {
        return 0;
    }

    SC2APIProtocol::Response* response = new SC2APIProtocol::Response();
    if (!response->ParseFromArray(data, (int)data_len)) {
        return 1;
    }

    sc2_connection->PushResponse(response);

    return 1;
}

static void ConnectionClosedHandler(const struct mg_connection* conn, void *) {
    sc2::Connection* sc2_connection;
    if (!GetClientData(conn, sc2_connection)) {
        return;
    }

    if (sc2_connection->connection_closed_callback_) {
        sc2_connection->connection_closed_callback_();
    }
}

Connection::Connection() :
    connection_(nullptr),
    verbose_(false),
    queue_(),
    mutex_(),
    condition_(),
    has_response_(false) {}


bool Connection::Connect(const std::string& address, int port, bool verbose) {
    StartCivetweb();
    verbose_ = verbose;

    char ebuff[100] = { 0 };

    connection_ = mg_connect_websocket_client(
        address.c_str(),
        port,
        0,
        ebuff,
        100,
        "/sc2api",
        nullptr,
        DataHandler,
        ConnectionClosedHandler,
        (void*) this);

    if (!connection_) {
        return false;
    }

    if (verbose_) {
        std::cout << "Connected..." << std::endl;
    }

    return true;
}

Connection::~Connection() {
    Disconnect();
}

void Connection::Send(const SC2APIProtocol::Request* request) {
    if (!request) {
        return;
    }
    // Connection must be established before sending.
    assert(connection_);
    if (!connection_) {
        return;
    }
    size_t size = request->ByteSize();
    void* buffer = malloc(size);
    request->SerializeToArray(buffer, (int)size);
    mg_websocket_write(
        connection_,
        MG_WEBSOCKET_OPCODE_BINARY,
        (const char*) buffer,
        size);

    free(buffer);

    if (verbose_) {
        std::cout << "Sending: " << request->DebugString();
    }
}

bool Connection::Receive(
    SC2APIProtocol::Response*& response,
    unsigned int timeout_ms) {
    std::unique_lock<std::mutex> lock(mutex_);
    // Block until a message is recieved.
    if (verbose_) {
        std::cout << "Waiting for response..." << std::endl;
    }
    auto now = std::chrono::system_clock::now();
    if (condition_.wait_until(
        lock,
        now + std::chrono::milliseconds(timeout_ms),
        [&] { return queue_.size() != 0; })) {
        lock.unlock();
        PopResponse(response);
        return true;
    }

    lock.unlock();
    response = nullptr;
    Disconnect();
    queue_.clear();

    // Execute the timeout callback if it exists.
    if (timeout_callback_) {
        timeout_callback_();
    }

    return false;
}

void Connection::PushResponse(SC2APIProtocol::Response*& response) {
    std::lock_guard<std::mutex> guard(mutex_);
    queue_.push_back(response);
    condition_.notify_one();
    has_response_ = true;
}

void Connection::PopResponse(SC2APIProtocol::Response*& response) {
    if (queue_.empty()) return;
    std::lock_guard<std::mutex> guard(mutex_);
    response = queue_.front();
    queue_.pop_front();
    if (queue_.empty()) {
        has_response_ = false;
    }
}

void Connection::SetTimeoutCallback(std::function<void()> callback) {
    timeout_callback_ = callback;
}

void Connection::SetConnectionClosedCallback(std::function<void()> callback) {
    connection_closed_callback_ = callback;
}

bool Connection::HasConnection() const {
    return connection_ != nullptr;
}

void Connection::Disconnect() {
    mg_close_connection(connection_);
    connection_ = nullptr;
}

bool Connection::PollResponse() {
    return has_response_;
}

}
