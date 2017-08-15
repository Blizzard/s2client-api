#include "sc2api/sc2_coordinator.h"
#include "sc2api/sc2_agent.h"
#include "sc2api/sc2_replay_observer.h"
#include "sc2api/sc2_args.h"
#include "sc2api/sc2_interfaces.h"
#include "sc2api/sc2_control_interfaces.h"

#include "sc2utils/sc2_manage_process.h"
#include "sc2utils/sc2_scan_directory.h"

#include "sc2api.pb.h"

#include <iostream>
#include <fstream>
#include <cassert>
#include <thread>

namespace sc2 {

void RunParallel(const std::function<void(Agent* a)>& step, std::vector<Agent*>& agents) {
    // Run all steps in parallel.
    std::vector<std::thread> threads(agents.size());
    for (size_t i = 0; i < agents.size(); ++i) {
        Agent* a = agents[i];
        threads[i] = std::thread(std::bind(step, a));
    }

    for (auto& t : threads) {
        t.join();
    }
}

int LaunchProcess(ProcessSettings& process_settings, Client* client, int window_width, int window_height, int window_start_x, int window_start_y, int port, int client_num=0) {
    assert(client);
    process_settings.process_info.push_back(sc2::ProcessInfo());
    ProcessInfo& pi = process_settings.process_info.back();

    // Get the next port
    pi.port = port;

    // Command line arguments that will be passed to sc2.
    std::vector<std::string> cl = {
        "-listen", process_settings.net_address,
        "-port", std::to_string(pi.port)
    };

    // DirectX will fail if multiple games try to launch in fullscreen mode. Force them into windowed mode.
    cl.push_back("-displayMode"); cl.push_back("0");

    // TODO: This is deprecated. Remove this.
    cl.push_back("-simulationSpeed");
    if (process_settings.realtime) {
        cl.push_back("1");
    }
    else {
        cl.push_back("0");
    }

    for (const std::string& command : process_settings.extra_command_lines)
        cl.push_back(command);

    cl.push_back("-windowwidth"); cl.push_back(std::to_string(window_width));
    cl.push_back("-windowheight"); cl.push_back(std::to_string(window_height));

    if (client_num < 2) {
        cl.push_back("-windowx"); cl.push_back(std::to_string(window_start_x + window_width * client_num));
        cl.push_back("-windowy"); cl.push_back(std::to_string(window_start_y));
    }
    else if (client_num < 4) {
        cl.push_back("-windowx"); cl.push_back(std::to_string(window_start_x + window_width * (client_num - 2)));
        cl.push_back("-windowy"); cl.push_back(std::to_string(window_start_y + window_height));
    }

    pi.process_path = process_settings.process_path;
    pi.process_id = StartProcess(process_settings.process_path, cl);
    if (!pi.process_id) {
        std::cerr << "Unable to start sc2 executable with path: "
            << process_settings.process_path
            << std::endl;
    }
    else {
        std::cout << "Launched SC2 (" << process_settings.process_path << "), PID: " << std::to_string(pi.process_id) << std::endl;
    }

    client->Control()->SetProcessInfo(pi);
    return pi.port;
}

int LaunchProcesses(ProcessSettings& process_settings, std::vector<Client*> clients, int window_width, int window_height, int window_start_x, int window_start_y) {
    int last_port = 0;
    // Start an sc2 process for each bot.
    int clientIndex = 0;
    for (auto c : clients) {
        last_port = LaunchProcess(process_settings, 
            c, 
            window_width, 
            window_height, 
            window_start_x, 
            window_start_y, 
            process_settings.port_start + static_cast<int>(process_settings.process_info.size()) - 1, 
            clientIndex++);
    }

    // Since connect is blocking do it after the processes are launched.
    for (std::size_t i = 0; i < clients.size(); ++i) {
        const ProcessInfo& pi = process_settings.process_info[i];
        Client* c = clients[i];

        assert(pi.process_id && IsProcessRunning(pi.process_id));
        bool connected = c->Control()->Connect(process_settings.net_address, pi.port, process_settings.timeout_ms);
        assert(connected);
    }

    return last_port;
}


void SetupPorts(GameSettings& game_settings, std::vector<Agent*>& agents, int port_start) {
    // Join the game if there are two human participants.
    int humans = 0;
    for (const auto& p_setup : game_settings.player_setup) {
        if (p_setup.type == sc2::PlayerType::Participant) {
            ++humans;
        }
    }

    if (humans > 1) {
        game_settings.ports.shared_port = ++port_start;
        game_settings.ports.server_ports.game_port = ++port_start;
        game_settings.ports.server_ports.base_port = ++port_start;
        for (size_t i = 1; i < agents.size(); ++i) {
            PortSet port_set;
            port_set.game_port = ++port_start;
            port_set.base_port = ++port_start;
            game_settings.ports.client_ports.push_back(port_set);
        }
    }
}

static void CallOnStep(Agent* a) {
    ControlInterface* control = a->Control();
    if (!control->IsInGame()) {
        a->OnGameEnd();
        control->RequestLeaveGame(); // Only for multiplayer.
        return;
    }

    ActionInterface* action = a->Actions();
    control->IssueEvents(action->Commands());
    if (action) {
        action->SendActions();
    }

    ActionFeatureLayerInterface* action_feature_layer = a->ActionsFeatureLayer();
    if (action_feature_layer) {
        action_feature_layer->SendActions();
    }
}

// Implementation.
class CoordinatorImp {
public:
    std::vector<Agent*> agents_;
    std::vector<ReplayObserver*> replay_observers_;

    bool game_ended_;
    bool starcraft_started_;

    GameSettings game_settings_;
    ReplaySettings replay_settings_;
    InterfaceSettings interface_settings_;
    ProcessSettings process_settings_;

    CoordinatorImp();
    ~CoordinatorImp();

    bool StartGame();
    void StartReplay();

    void StepAgents();
    void StepAgentsRealtime();
    void StepReplayObservers();

    bool AnyObserverAvailable() const;

    bool WaitForAllResponses();
    void AddAgent(Agent* agent);

    int window_width_ = 1024;
    int window_height_ = 768;

    int window_start_x_ = 100;
    int window_start_y_ = 200;

    // If a ReplayObserver fails, try to recover
    bool replay_recovery_ = true;
    int last_port_ = 0;
};

CoordinatorImp::CoordinatorImp() :
    agents_(),
    replay_observers_(),
    game_ended_(),
    starcraft_started_(false),
    game_settings_(),
    process_settings_(false, 1, "", "127.0.0.1", kDefaultProtoInterfaceTimeout, 8168, false) {
}

CoordinatorImp::~CoordinatorImp() {
    for (auto& p : process_settings_.process_info) {
        TerminateProcess(p.process_id);
    }
}

bool CoordinatorImp::AnyObserverAvailable() const {
    for (auto r : replay_observers_) {
        if (!r->Control()->IsInGame()) {
            return true;
        }
    }

    return false;
}

void CoordinatorImp::StartReplay() {
    // If no replays given in the settings don't try.
    if (replay_settings_.replay_file.empty()) {
        return;
    }

    assert(!replay_observers_.empty());
    if (!starcraft_started_) {
        last_port_ = LaunchProcesses(process_settings_,
            std::vector<sc2::Client*>(replay_observers_.begin(), replay_observers_.end()), window_width_, window_height_, window_start_x_, window_start_y_);
    }

    // Run a replay with each available replay observer.
    for (auto r : replay_observers_) {
        bool loaded_replay = false;
        while (!loaded_replay) {
            if (replay_settings_.replay_file.empty()) {
                break;
            }

            const std::string& file = replay_settings_.replay_file.front();

            if (!file.empty()) {
                // If the replay observer is idle or out of game use it for a new replay.
                if (!r->Control()->IsReadyForCreateGame()) {
                    loaded_replay = true;
                    continue;
                }

                // Gather replay information with the available observer.
                r->ReplayControl()->GatherReplayInfo(file);

                // If the replay isn't being pruned based on replay info start it.
                if (!r->IgnoreReplay(r->ReplayControl()->GetReplayInfo(), replay_settings_.player_id)) {
                    loaded_replay = r->ReplayControl()->LoadReplay(file, interface_settings_, replay_settings_.player_id);
                }
            }

            // Front to back ordering more important than remove from back speedup.
            replay_settings_.replay_file.erase(
                replay_settings_.replay_file.begin(), replay_settings_.replay_file.begin() + 1);
        }
    }

    starcraft_started_ = true;
}

void CoordinatorImp::StepAgents() {
    auto step_agent = [this](Agent* a) {
        ControlInterface* control = a->Control();

        if (control->GetAppState() != AppState::normal) {
            return;
        }

        if (control->PollLeaveGame()) {
            return;
        }

        if (control->IsFinishedGame()) {
            return;
        }

        control->Step(process_settings_.step_size);
        control->WaitStep();
        if (process_settings_.multi_threaded) {
            CallOnStep(a);
        }
    };

    if (agents_.size() == 1) {
        step_agent(agents_.front());
    }
    else {
        RunParallel(step_agent, agents_);
    }

    if (!process_settings_.multi_threaded) {
        for (auto a : agents_) {
            if (a->Control()->GetAppState() != AppState::normal) {
                continue;
            }

            // It is possible to have a pending leave game request here.
            if (a->Control()->PollLeaveGame()) {
                continue;
            }

            CallOnStep(a);
        }
    }

}

void CoordinatorImp::StepAgentsRealtime() {
    auto step_agent = [](Agent* a) {
        ControlInterface* control = a->Control();
        if (!control) {
            return;
        }

        if (control->GetAppState() != AppState::normal) {
            return;
        }

        if (control->PollLeaveGame()) {
            return;
        }

        if (a->Control()->IsFinishedGame()) {
            return;
        }

        ActionInterface* action = a->Actions();
        if (!action) {
            return;
        }

        // This agent shouldn't call step since it's real time.
        control->GetObservation();
        control->IssueEvents(a->Actions()->Commands());
        action->SendActions();

        if (!control->IsInGame()) {
            a->OnGameEnd();
            a->Control()->RequestLeaveGame(); // Only for multiplayer.
            return;
        }
    };

    if (process_settings_.multi_threaded) {
        RunParallel(step_agent, agents_);
    }
    else {
        for (auto a : agents_) {
            step_agent(a);
        }
    }
}

void CoordinatorImp::StepReplayObservers() {
    // Run all replay observers.
    auto run_replay = [this](ReplayObserver* r) {
        if (r->Control()->GetAppState() != AppState::normal) {
            return;
        }

        // If the replay is loading wait for it to finish loading before performing a step.
        if (r->Control()->HasResponsePending()) {
            // Don't consume a response if there isn't one in the queue.
            if (replay_observers_.size() > 1 && !r->Control()->PollResponse()) {
                return;
            }
            r->ReplayControl()->WaitForReplay();
        }

        if (r->Control()->IsInGame()) {
            r->Control()->Step(process_settings_.step_size);
            r->Control()->WaitStep();

            // If multithreaded run everyones OnStep in parallel.
            if (process_settings_.multi_threaded) {
                r->Control()->IssueEvents();
            }

            if (!r->Control()->IsInGame()) {
                r->OnGameEnd();
            }
        }
    };

    if (replay_observers_.size() == 1) {
        run_replay(replay_observers_.front());
    }
    else {
        // Run all steps in parallel.
        std::vector<std::thread> threads(replay_observers_.size());
        for (size_t i = 0; i < replay_observers_.size(); ++i) {
            ReplayObserver* r = replay_observers_[i];
            threads[i] = std::thread(std::bind(run_replay, r));
        }

        // Join all threads.
        for (size_t i = 0; i < threads.size(); ++i) {
            threads[i].join();
        }
    }

    // Do everyones OnStep, if not multi threaded, in single threaded mode.
    if (!process_settings_.multi_threaded) {
        for (auto r : replay_observers_) {
            if (r->Control()->GetAppState() != AppState::normal) {
                continue;
            }

            r->Control()->IssueEvents();
        }
    }
}

bool CoordinatorImp::WaitForAllResponses() {
    static const int sleep_ms = 50;

    int total_sleep_ms = 0;
    for (;;) {
        bool has_responses = false;

        for (Agent* agent : agents_) {
            if (!agent->Control()->HasResponsePending() || agent->Control()->GetAppState() != AppState::normal) {
                continue;
            }

            has_responses = true;

            if (agent->Control()->PollResponse()) {
                agent->Control()->ConsumeResponse();
            }

            break;
        }

        for (ReplayObserver* replay_observer : replay_observers_) {
            if (has_responses) {
                break;
            }

            if (!replay_observer->Control()->HasResponsePending() || replay_observer->Control()->GetAppState() != AppState::normal) {
                continue;
            }
            has_responses = true;

            if (replay_observer->Control()->PollResponse()) {
                replay_observer->Control()->ConsumeResponse();
            }

            break;
        }

        if (!has_responses) {
            break;
        }

        if (total_sleep_ms >= process_settings_.timeout_ms) {
            assert(0);
            return false;
        }

        SleepFor(sleep_ms);
        total_sleep_ms += sleep_ms;
    }

    return true;
}

bool CoordinatorImp::StartGame() {
    assert(starcraft_started_);

    // Create the game with the first client.
    Agent* firstClient = agents_.front();
    bool is_game_created = firstClient->Control()->CreateGame(game_settings_.map_name, game_settings_.player_setup, process_settings_.realtime);
    assert(is_game_created);

    int i = 0;
    for (auto c : agents_) {
        bool game_join_request = c->Control()->RequestJoinGame(game_settings_.player_setup[i++],
            interface_settings_,
            game_settings_.ports);

        assert(game_join_request);
    }

    for (auto c : agents_) {
        c->Control()->WaitJoinGame();
    }

    // Check if any errors occurred during game start.
    bool errors_occurred = false;
    for (auto c : agents_) {
        ControlInterface* control = c->Control();
        const std::vector<ClientError>& client_errors = control->GetClientErrors();
        if (!client_errors.empty()) {
            c->OnError(client_errors, control->GetProtocolErrors());
            errors_occurred = true;
        }
    }

    if (errors_occurred) {
        return false;
    }

    // Run all clients on game start.
    for (auto c : agents_) {
        c->Control()->GetObservation();
    }
    for (auto c : agents_) {
        c->OnGameFullStart();
    }
    for (auto c : agents_) {
        c->Control()->OnGameStart();
        c->OnGameStart();
    }
    for (auto c : agents_) {
        c->Control()->IssueEvents(c->Actions()->Commands());
    }

    return true;
}

// Coordinator.

Coordinator::Coordinator() {
    imp_ = new CoordinatorImp();
}

Coordinator::~Coordinator() {
    delete imp_;
}

bool Coordinator::StartGame(const std::string& map_path) {
    imp_->game_settings_.map_name = map_path;
    return imp_->StartGame();
}

bool Coordinator::RemoteSaveMap(const void* data, int data_size, std::string remote_path) {
    for (auto c : imp_->agents_) {
        if (!c->Control()->RemoteSaveMap(data, data_size, remote_path))
            return false;
    }

    for (auto c : imp_->replay_observers_) {
        if (!c->Control()->RemoteSaveMap(data, data_size, remote_path))
            return false;
    }

    return true;
}

void Coordinator::SetParticipants(const std::vector<PlayerSetup>& participants) {
    imp_->game_settings_.player_setup.clear();
    imp_->agents_.clear();

    for (const auto& p : participants) {
        if (p.agent) {
            imp_->AddAgent(p.agent);
        }
        imp_->game_settings_.player_setup.push_back(p);
    }
}

void Coordinator::SetReplayRecovery(bool value) {
    imp_->replay_recovery_ = value;
}


bool Coordinator::LoadSettings(int argc, char** argv) {
    return ParseSettings(argc, argv, imp_->process_settings_, imp_->game_settings_);
}

void Coordinator::LaunchStarcraft() {
    if (!DoesFileExist(imp_->process_settings_.process_path)) {
        std::cerr << "Executable path can't be found, try running the StarCraft II executable first." << std::endl;
        if (!imp_->process_settings_.process_path.empty()) {
            std::cerr << imp_->process_settings_.process_path << " does not exist on your filesystem.";
        }
        std::cerr << std::endl;
        assert(!"Could not find the executable. Supply a valid path.");
        exit(1);
    }

    assert(!imp_->agents_.empty());

    // TODO: Check the case that a pid in the process_info_ struct is no longer running.
    // The process may have died.
    int port_start = 0;
    if (imp_->process_settings_.process_info.size() != imp_->agents_.size()) {
        port_start = LaunchProcesses(imp_->process_settings_,
            std::vector<sc2::Client*>(imp_->agents_.begin(), imp_->agents_.end()), imp_->window_width_, imp_->window_height_, imp_->window_start_x_, imp_->window_start_y_);
    }

    SetupPorts(imp_->game_settings_, imp_->agents_, port_start);

    imp_->starcraft_started_ = true;
    imp_->last_port_ = port_start;
}

void Coordinator::LeaveGame() {
    for (auto c : imp_->agents_) {
        c->Control()->RequestLeaveGame();
    }
}

void Coordinator::WaitForAllResponses() {
    imp_->WaitForAllResponses();
}

bool Coordinator::Update() {
    if (imp_->agents_.size() > 0) {
        if (imp_->process_settings_.realtime) {
            imp_->StepAgentsRealtime();
        }
        else {
            imp_->StepAgents();
        }
    }

    if (imp_->replay_observers_.size() > 0 && imp_->starcraft_started_) {
        if (imp_->process_settings_.realtime) {
            // TODO
            assert(0);
        }
        else {
            imp_->StepReplayObservers();
        }
    }

    if (imp_->replay_observers_.size() > 0) {
        if (imp_->AnyObserverAvailable()) {
            imp_->StartReplay();
        }
    }

    // Check for errors in all agents/replay observers at the end of an update.
    bool error_occurred = false;
    for (auto agent : imp_->agents_) {
        const ControlInterface* control = agent->Control();
        const std::vector<ClientError>& client_errors = control->GetClientErrors();
        if (!client_errors.empty()) {
            agent->OnError(client_errors, control->GetProtocolErrors());
            error_occurred = true;
        }
    }

    for (auto replay_observer : imp_->replay_observers_) {
        ControlInterface* control = replay_observer->Control();
        const std::vector<ClientError>& client_errors = control->GetClientErrors();
        if (!client_errors.empty()) {
            replay_observer->OnError(client_errors, control->GetProtocolErrors());
            error_occurred = true;
            if (imp_->replay_recovery_) {
                const ProcessInfo& pi = control->GetProcessInfo();

                // Try to kill SC2 then relaunch it
                sc2::TerminateProcess(pi.process_id);

                // Reset the control interface so internal state gets reset.
                replay_observer->Reset();

                // ReplayObserver needs the control interface from Client.
                replay_observer->SetControl(replay_observer->Control());

                // Control interface has been reconstructed.
                control = replay_observer->Control();

                imp_->last_port_ = LaunchProcess(imp_->process_settings_, 
                    replay_observer, 
                    imp_->window_width_, 
                    imp_->window_height_, 
                    imp_->window_start_x_, 
                    imp_->window_start_y_, 
                    imp_->last_port_ + 1);

                const ProcessInfo& pi_new = control->GetProcessInfo();

                bool connected = control->Connect(imp_->process_settings_.net_address, pi_new.port, imp_->process_settings_.timeout_ms);

                // An error did occur but if we succesfully recovered ignore it. The client will still gets its event
                if (connected) {
                    error_occurred = false;
                }
            }
        }
    }


    // End the coordinator update on the idea that an error in the API should mean it's time to stop.
    if (error_occurred) {
        return false;
    }

    return !AllGamesEnded();
}

bool Coordinator::AllGamesEnded() const {
    for (auto a : imp_->agents_) {
        if (a->Control()->IsInGame() || a->Control()->HasResponsePending()) {
            return false;
        }
    }

    for (auto r : imp_->replay_observers_) {
        if (r->Control()->IsInGame() || r->Control()->HasResponsePending()) {
            return false;
        }
    }

    return true;
}

void CoordinatorImp::AddAgent(Agent* agent) {
    assert(agent);
    agents_.push_back(agent);
}

void Coordinator::AddReplayObserver(ReplayObserver* replay_observer) {
    assert(replay_observer);
    imp_->replay_observers_.push_back(replay_observer);
}

void Coordinator::SetMultithreaded(bool value) {
    imp_->process_settings_.multi_threaded = value;
}

void Coordinator::SetRealtime(bool value) {
    // Realtime must be set before LaunchStarcraft is called.
    assert(!imp_->starcraft_started_);
    imp_->process_settings_.realtime = value;
}

void Coordinator::SetStepSize(int step_size) {
    if (step_size < 1) {
        assert(0);
        return;
    }

    // SetStepSize must be set before LaunchStarcraft is called.
    assert(!imp_->starcraft_started_);
    imp_->process_settings_.step_size = step_size;
}

void Coordinator::SetProcessPath(const std::string& path) {
    assert(!imp_->starcraft_started_);
    imp_->process_settings_.process_path = path;
}

void Coordinator::SetTimeoutMS(uint32_t timeout_ms) {
    imp_->process_settings_.timeout_ms = timeout_ms;
}

void Coordinator::SetPortStart(int port_start) {
    assert(!imp_->starcraft_started_);
    imp_->process_settings_.port_start = port_start;
}

void Coordinator::SetFeatureLayers(const FeatureLayerSettings& settings) {
    // Feature Layers must be set before LaunchStarcraft is called.
    assert(!imp_->starcraft_started_);
    imp_->interface_settings_.use_feature_layers = true;
    imp_->interface_settings_.feature_layer_settings= settings;
}

void Coordinator::SetRender(const RenderSettings& settings) {
    // Render must be set before LaunchStarcraft is called.
    assert(!imp_->starcraft_started_);
    imp_->interface_settings_.use_render = true;
    imp_->interface_settings_.render_settings = settings;
}

void Coordinator::SetWindowSize(int width, int height) {
    assert(!imp_->starcraft_started_);
    imp_->window_width_ = width;
    imp_->window_height_ = height;
}

void Coordinator::SetWindowLocation(int x, int y) {
    assert(!imp_->starcraft_started_);
    imp_->window_start_x_ = x;
    imp_->window_start_y_ = y;
}

bool Coordinator::SetReplayPath(const std::string& path) {
    imp_->replay_settings_.replay_file.clear();

    if (HasExtension(path, ".SC2Replay")) {
        imp_->replay_settings_.replay_file.push_back(path);
    }
    else {
        imp_->replay_settings_.replay_dir = path;

        // Gather and append all files from the directory.
        if (!imp_->replay_settings_.replay_dir.empty()) {
            scan_directory(imp_->replay_settings_.replay_dir.c_str(), imp_->replay_settings_.replay_file, true);
        }
    }

    return !imp_->replay_settings_.replay_file.empty();
}

bool Coordinator::LoadReplayList(const std::string& file_path) {
    if (!DoesFileExist(file_path))
        return false;

    imp_->replay_settings_.replay_file.clear();

    std::ifstream replay_file(file_path);

    std::string line;
    while (std::getline(replay_file, line)) {
        if (line.length() < 5)
            continue;

        imp_->replay_settings_.replay_file.push_back(line);
    }

    return true;
}

void Coordinator::SaveReplayList(const std::string& file_path) {
    std::ofstream replay_file(file_path, std::ofstream::out | std::ofstream::trunc);
    for (const std::string& line : imp_->replay_settings_.replay_file) {
        replay_file << line << std::endl;
    }
}

bool Coordinator::HasReplays() const {
    return !imp_->replay_settings_.replay_file.empty();
}

void Coordinator::AddCommandLine(const std::string& option) {
    imp_->process_settings_.extra_command_lines.push_back(option);
}

std::string Coordinator::GetExePath() const {
    if (imp_->process_settings_.process_path.length() > 4)
        return imp_->process_settings_.process_path;

    return imp_->process_settings_.process_path;
}

}
