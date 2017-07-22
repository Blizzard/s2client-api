/*! \file sc2_coordinator.h
    \brief Frontend for running a game.

    The coordinator acts as a game and bot manager. It is used to launch StarCraft and setup proto connections
    between a user's bot and the running StarCraft instance. With it a user steps forward a simulation and it will
    fill out bot interface data.
*/

#pragma once

#include <vector>
#include <string>
#include "sc2api/sc2_game_settings.h"
#include "sc2api/sc2_proto_interface.h"

namespace sc2 {

class Agent;
class ReplayObserver;
class CoordinatorImp;

//! Coordinator of one or more clients. Used to start, step and stop games and replays.
class Coordinator {
public:
    Coordinator();
    ~Coordinator();

    // Initialization and setup.

    //! Used to load settings. Settings will be discovered in the following order:
    //!     1. If command line arguments are provided it will use them. Invoke binary with --help to see expected arguments.
    //!     2. (Recommended) If the StarCraft II binary has been run the function will auto discover its location.
    //! \param argc Provided in main signature.
    //! \param argv Provided in main signature.
    //! \param game_settings The name of the settings file.
    //! \return True if settings were found or discovered, false otherwise.
    bool LoadSettings(int argc, char** argv);

    //! Specifies whether bots or replays OnStep function should be run in parallel. If set to true make sure your bots are thread-safe
    //! if they reach into shared code.
    //! \param value True to multithread, false otherwise.
    void SetMultithreaded(bool value);

    //! Specifies whether the game should run in realtime or not. If the game is running in real time that means the coordinator is
    //! not stepping it forward. The game is running and your bot reaches into it asynchronously to read state.
    //! \param value True to be realtime, false otherwise.
    void SetRealtime(bool value);

    //! Sets the number of game loops to run for each step.
    //! \param step_size Number of gameloops to run for each step.
    void SetStepSize(int step_size);

    //! Sets the path to the StarCraft II binary.
    //! \param path Absolute file path.
    void SetProcessPath(const std::string& path);

    //! Sets the timeout for network operations.
    //! \param value timeout_ms in milliseconds.
    void SetTimeoutMS(uint32_t timeout_ms = kDefaultProtoInterfaceTimeout);

    //! Sets the first port number to use. Subsequent port assignments are sequential.
    //! \param port_start First port number.
    void SetPortStart(int port_start);

    //! Indicates whether feature layers should be provided in the observation.
    //! \param settings Configuration of feature layer settings.
    //! \sa FeatureLayerSettings
    void SetFeatureLayers(const FeatureLayerSettings& settings);

    //!
    //! \sa RenderSettings
    void SetRender(const RenderSettings& settings);

    //! Sets the game window dimensions.
    //! \param width Width of game window.
    //! \param height Height of game window.
    void SetWindowSize(int width, int height);

    //! Sets the game window location.
    //! \param x X position of game window.
    //! \param y y position of game window.
    void SetWindowLocation(int x, int y);

    //! Appends a command line argument to be fed to StarCraft II when starting.
    // \param option The string to be appended to the executable invoke.
    void AddCommandLine(const std::string& option);

    //! Sets up the bots and whether they are controlled by in-built AI, human or a custom bot.
    // \param participants A vector of player setups for each participant in the game.
    // \sa PlayerSetup
    void SetParticipants(const std::vector<PlayerSetup>& participants);

    void SetReplayRecovery(bool value);

    //! Add an instance of ReplayObserver, each ReplayObserver will run a separate StarCraft II client.
    // \param replay_observer A pointer to the replay observer to utilize.
    // \sa ReplayObserver
    void AddReplayObserver(ReplayObserver* replay_observer);

    // Start-up.

    //! Uses settings gathered from LoadSettings, specifically the path to the executable, to run StarCraft II.
    void LaunchStarcraft();

    //! Starts a game on a certain map. There are multiple ways to specify a map:
    //! Absolute path: Any .SC2Map file.
    //! Relative path: Any .SC2Map file relative to either the library or installation maps folder.
    //! Map name: Any BattleNet published map that has been locally cached.
    //! \param map_path Path to the map to run.
    //! \return True if the game started, false if there was errors or the game didn't start, override OnError callback to see the exact errors.
    bool StartGame(const std::string& map_path = std::string());

    // Run.

    //! Helper function used to actually run a bot. This function will behave differently in real-time compared to
    //! non real-time. In real-time there is no step sent over the wire but instead will request and read observations as the game runs.
    //! * For non-real time Update will perform the following:
    //!     1. Step the simulation forward by a certain amount of game steps, this essentially moves the game loops forward.
    //!     2. Wait for the step to complete, the step is completed when a response is received and read from the StarCraft II binary.
    //!         * When the step is completed an Observation has been received. It is parsed and various client events are dispatched.
    //!     3. Call the user's OnStep function.
    //! * Real time applications will perform the following:
    //!     1. The Observation is directly requested. The process will block while waiting for it.
    //!     2. The Observation is parsed and client events are dispatched.
    //!     3. Unit actions batched from the ActionInterface are dispatched.
    //! \return True if the game has ended, false otherwise.
    bool Update();

    //! Requests for the currently running game to end.
    void LeaveGame();

    // Status.

    //! Returns true if all running games have ended.
    bool AllGamesEnded() const;

    // Replay specific.
    //! Sets the path for to a folder of replays to analyze.
    // \param path The folder path.
    bool SetReplayPath(const std::string& path);
    //! Loads replays from a file.
    // \param path The file path.
    bool LoadReplayList(const std::string& file_path);
    //! Saves replays to a file.
    // \param path The file path.
    void SaveReplayList(const std::string& file_path);
    //! Determines if there are unprocessed replays.
    //!< \return Is true if there are replays left.
    bool HasReplays() const;

    // Misc.

    //! Blocks for all bots to receive any pending responses
    void WaitForAllResponses();
    //! Saves a binary blob as a map to a remote location.
    // \param data The map data.
    // \param data_size The size of map data.
    // \param remote_path The file path to save the data to.
    //!< \return Is true if the save is successful.
    bool RemoteSaveMap(const void* data, int data_size, std::string remote_path);
    //! Gets the game executable path.
    //!< \return The game executable path.
    std::string GetExePath() const;

private:
    CoordinatorImp* imp_;
};

}
