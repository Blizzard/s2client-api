/*! \file sc2_connection.h
    \brief A basic websocket connection and queue wrapper.
*/

#pragma once

#include <string>
#include <mutex>
#include <condition_variable>
#include <deque>
#include <functional>
#include <atomic>

struct mg_connection;

namespace SC2APIProtocol {
    class Request;
    class Response;
}

namespace sc2 {


//! This class acts as a wrapper around a websocket connection and queue responsible for both sending
//! out and receiving protobuf messages.
class Connection {
public:
    Connection();
    ~Connection();

    //! Connects via websocket on a given address/port.
    //!< \param address The address to connect to, will most commonly be used locally so 127.0.0.1.
    //!< \param port The port to connect the, the default for s2api is 9168 unless specified otherwise in settings.
    //!< \return Returns true if the connection was successful and false otherwise.
    bool Connect(const std::string& address, int port, bool verbose = true);

    //! Sends a request via the websocket connection. This function assumes Connect has been called and returned success.
    //! It will assert in debug if that's not the case and will early out in a build that doesn't have asserts built in.
    //! This function also allocates a byte buffer to accommodate the request, it frees that buffer before returning.
    //!< \param request A pointer to the Request object.
    void Send(const SC2APIProtocol::Request* request);

    //! Receive will block until a message is received from its websocket connection. If a message is not received within
    //! the timeout it will set response to null and return false, it also calls a timeout callback that can be used if
    //! a user has any timeout logic.
    //! \param response The response pointer to be filled out.
    //! \timeout_ms The max time, in milliseconds, the function will wait to receive a message.
    //! \return Returns true if a message is received, false otherwise.
    bool Receive(SC2APIProtocol::Response*& response, unsigned int timeout_ms);

    //! PopResponse is called in the Receive function when a message has been received off of the civetweb thread. Alternatively
    //! you could poll for responses with PollResponse and consume the message manually with this function.
    //! \param response The response pointer to be filled out.
    void PopResponse(SC2APIProtocol::Response*& response);

    //! An accessor function that a user can bind a timeout function to.
    //! \param callback A functor or lambda that represents the callback.
    void SetTimeoutCallback(std::function<void()> callback);

    void SetConnectionClosedCallback(std::function<void()> callback);

    //! Whether or not the connection is valid.
    //!< \return true if the connection is valid, false otherwise.
    bool HasConnection() const;

    //! Disconnect from the game if currently connected.
    void Disconnect();

    //! Polls the queue in a thread safe way to check if a message has been received.
    //!< \return true if there is a response in the queue, false otherwise.
    bool PollResponse();

    //! PushResponse is called by a civetweb thread when it receives a message off the socket. Pushing a response triggers
    //! a condition and enqueues a message. The condition will cause anyone currently blocking for a response (if Receive is called)
    //! to wake up and be able to consume that message.
    //! \param response A pointer to the Response to queue.
    void PushResponse(SC2APIProtocol::Response*& response);


    std::function<void()> timeout_callback_;             //!< Timeout callback.
    std::function<void()> connection_closed_callback_;   //!< Timeout callback.

    mg_connection* connection_;                          //!< A pointer to the civetweb connection object.

private:
    bool verbose_;                                   //!< Will print extra information to console if enabled.

    std::deque<SC2APIProtocol::Response*> queue_; //!< A queue that contains responses received off the socket.
    std::mutex mutex_;                               //!< Mutex used in conjunction with the condition.
    std::condition_variable condition_;              //!< A condition that is signaled when a message has been received off the socket.

    std::atomic_bool has_response_;                  //!< Thread safe bool to check whether the queue is not empty.
};

}
