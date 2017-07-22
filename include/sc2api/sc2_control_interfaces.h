#pragma once

#include "sc2api/sc2_data.h"
#include "sc2api/sc2_proto_interface.h"
#include "sc2api/sc2_client.h"
#include "sc2api/sc2_game_settings.h"

#include <vector>
#include <string>

namespace sc2 {

struct ProcessInfo;
struct InterfaceSettings;

class ControlInterface {
public:
    virtual ProtoInterface& Proto() = 0;
    virtual bool Connect(const std::string& address, int port, int timeout_ms) = 0;
    virtual bool RemoteSaveMap(const void* data, int data_size, std::string remote_path) = 0;
    virtual bool CreateGame(const std::string& map_path, const std::vector<PlayerSetup>& players, bool realtime) = 0;

    virtual bool RequestJoinGame(PlayerSetup setup, const InterfaceSettings& settings, const Ports& ports = Ports()) = 0;
    virtual bool WaitJoinGame() = 0;

    virtual bool RequestLeaveGame() = 0;
    virtual bool PollLeaveGame() = 0;

    virtual bool Step(int count = 1) = 0;
    virtual bool WaitStep() = 0;

    virtual bool SaveReplay(const std::string& path) = 0;

    virtual bool Ping() = 0;

    // General.
    virtual GameResponsePtr WaitForResponse() = 0;
    virtual void SetProcessInfo(const ProcessInfo& pi) = 0;
    virtual const ProcessInfo& GetProcessInfo() const = 0;

    virtual AppState GetAppState() const = 0;

    virtual SC2APIProtocol::Status GetLastStatus() const = 0;
    virtual bool IsInGame() const = 0;
    virtual bool IsFinishedGame() const = 0;
    virtual bool IsReadyForCreateGame() const = 0;
    virtual bool HasResponsePending() const = 0;

    virtual bool GetObservation() = 0;
    virtual bool PollResponse() = 0;
    virtual bool ConsumeResponse() = 0;

    virtual bool IssueEvents(const std::vector<Tag>& commands = {}) = 0;
    virtual void OnGameStart() = 0;

    // Diagnostic.
    virtual void DumpProtoUsage() = 0;

    virtual void Error(ClientError error, const std::vector<std::string>& errors={}) = 0;
    virtual void ErrorIf(bool condition, ClientError error, const std::vector<std::string>& errors={}) = 0;

    virtual const std::vector<ClientError>& GetClientErrors() const = 0;
    virtual const std::vector<std::string>& GetProtocolErrors() const = 0;

    virtual void ClearClientErrors() = 0;
    virtual void ClearProtocolErrors() = 0;
};

class AgentControlInterface {
public:
    virtual bool Restart() = 0;
};

class ReplayControlInterface {
public:
    virtual bool GatherReplayInfo(const std::string& path) = 0;
    virtual bool LoadReplay(const std::string& replay_path, const InterfaceSettings& settings, uint32_t player_id) = 0;
    virtual bool WaitForReplay() = 0;

    virtual const ReplayInfo& GetReplayInfo() const = 0;
};

}
