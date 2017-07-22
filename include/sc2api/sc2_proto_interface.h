#pragma once

#include "sc2_connection.h"

#include "sc2api.pb.h"

#include <functional>

namespace sc2 {

class ControlInterface;

const unsigned int kDefaultProtoInterfaceTimeout = 120000; // A generous 120 seconds.

typedef std::shared_ptr<SC2APIProtocol::Request> GameRequestPtr;
typedef std::shared_ptr<const SC2APIProtocol::Response> GameResponsePtr;

template<class MessageType>
class MessageResponsePtr {
public:
    MessageResponsePtr() :
        message_(nullptr) {
    }

    void Set(GameResponsePtr response, const MessageType* message) {
        response_ = response;
        message_ = message;
    }

    bool HasErrors() const {
        if (!HasResponse())
            return true;
        if (response_->error_size() > 0)
            return true;
        if (!HasMessage())
            return true;
        return false;
    }

    void Clear() {
        message_ = nullptr;
        response_ = nullptr;
    }

    const MessageType* operator->() const {
        assert(message_);
        return message_;
    }

    const MessageType* get() const {
        assert(message_);
        return message_;
    }

    GameResponsePtr GetResponse() const {
        return response_;
    }

    bool HasResponse () const {
        return response_.get() != nullptr;
    }

    bool HasMessage () const {
        return message_ != nullptr;
    }

private:
    const MessageType* message_;
    GameResponsePtr response_;
};

#define SET_MESSAGE_RESPONSE(VAR, RESPONSE, MESSAGE_NAME)       \
    if (RESPONSE.get() && RESPONSE->has_##MESSAGE_NAME())       \
        VAR.Set(RESPONSE, &RESPONSE->MESSAGE_NAME());         \
    else                                                        \
        VAR.Set(RESPONSE, nullptr);

#define SET_SUBMESSAGE_RESPONSE(VAR, MESSAGE, SUBMESSAGE_NAME)          \
    if (!MESSAGE.HasErrors() && MESSAGE->has_##SUBMESSAGE_NAME())       \
        VAR.Set(MESSAGE.GetResponse(), &MESSAGE->SUBMESSAGE_NAME());  \
    else                                                                \
        VAR.Set(MESSAGE.GetResponse(), nullptr);

// Wraps proto and connections.
class ProtoInterface {
public:
    ProtoInterface();
    bool ConnectToGame(const std::string& address, int port, int timeout_ms);
    GameRequestPtr MakeRequest();
    bool SendRequest(GameRequestPtr& request, bool ignore_pending_requests = false);
    GameResponsePtr WaitForResponseInternal();
    bool PingGame();
    void Quit();
    void SetErrorCallback(std::function<void(const std::string& error_str)> error_callback);
    bool PollResponse();
    SC2APIProtocol::Status GetLastStatus() const { return latest_status_; }
    bool HasResponsePending() const;
    SC2APIProtocol::Response::ResponseCase GetResponsePending() const { return response_pending_; }
    int GetAssignedPort() const { return port_; }

    const std::vector<uint32_t>& GetStats() const { return count_uses_; }
    void SetControl(ControlInterface* control) { control_ = control; }

protected:
    Connection connection_;
    std::string address_;
    int port_;
    unsigned int default_timeout_ms_;
    std::function<void(const std::string& error_str)> error_callback_;
    SC2APIProtocol::Status latest_status_;
    SC2APIProtocol::Response::ResponseCase response_pending_;
    std::vector<uint32_t> count_uses_;
    ControlInterface* control_;
};

// Helper to produce a string for the proto type.
const char* RequestResponseIDToName(int type);

}
