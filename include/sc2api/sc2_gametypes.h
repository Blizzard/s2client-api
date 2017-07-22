/*! \file sc2_gametypes.h
    \brief Types used in setting up a game.
*/
#pragma once

#include <vector>
#include <string>

#include "sc2_common.h"

namespace sc2 {

typedef uint64_t Tag;
static const Tag NullTag = 0LL;

enum Race {
    Terran,
    Zerg,
    Protoss,
    Random
};

enum GameResult {
    Win,
    Loss,
    Tie,
    Undecided
};

enum Difficulty {
    VeryEasy = 1,
    Easy = 2,
    Medium = 3,
    MediumHard = 4,
    Hard = 5,
    HardVeryHard = 6,
    VeryHard = 7,
    CheatVision = 8,
    CheatMoney = 9,
    CheatInsane = 10
};

enum PlayerType {
    Participant = 1,
    Computer = 2,
    Observer = 3
};

class Agent;

//! Setup for a player in a game.
struct PlayerSetup {
    //! Player can be a Participant (usually an agent), Computer (in-built AI) or Observer.
    PlayerType type;
    //! Agent, if one is available.
    Agent* agent;

    // Only used for Computer

    //! Race: Terran, Zerg or Protoss. Only for playing against the built-in AI.
    Race race;
    //! Difficulty: Only for playing against the built-in AI.
    Difficulty difficulty;

    PlayerSetup() :
        type(Participant),
        agent(nullptr),
        race(Terran),
        difficulty(Easy) {
    };

    PlayerSetup(PlayerType in_type, Race in_race, Agent* in_agent = nullptr, Difficulty in_difficulty = Easy) :
        type(in_type),
        agent(in_agent),
        race(in_race),
        difficulty(in_difficulty) {

    }
};

static inline PlayerSetup CreateParticipant(Race race, Agent* agent) {
    return PlayerSetup(PlayerType::Participant, race, agent);
}

static inline PlayerSetup CreateComputer(Race race, Difficulty difficulty = Easy) {
    return PlayerSetup(PlayerType::Computer, race, nullptr, difficulty);
}

//! Port setup for a client.
struct PortSet {
    int game_port;
    int base_port;

    PortSet() :
        game_port(-1),
        base_port(-1) {
    }

    bool IsValid() const {
        return game_port > 0 && base_port > 0;
    }
};

//! Port setup for one or more clients in a game.
struct Ports {
    PortSet server_ports;
    std::vector<PortSet> client_ports;
    int shared_port;

    Ports() :
        shared_port(-1) {
    }

    bool IsValid() const {
        if (shared_port < 1)
            return false;
        if (!server_ports.IsValid())
            return false;
        if (client_ports.size() < 1)
            return false;
        for (std::size_t i = 0; i < client_ports.size(); ++i)
            if (!client_ports[i].IsValid())
                return false;

        return true;
    }
};

static const int max_path_size = 512;
static const int max_version_size = 32;
static const int max_num_players = 16;

//! Information about a player in a replay.
struct ReplayPlayerInfo {
    //! Player ID.
    int player_id;
    //! Player ranking.
    int mmr;
    //! Player actions per minute.
    int apm;
    //! Actual player race.
    Race race;
    //! Selected player race. If the race is "Random", the race data member may be different.
    Race race_selected;
    //! If the player won or lost.
    GameResult game_result;

    ReplayPlayerInfo() :
        player_id(0),
        mmr(-10000),
        apm(0),
        race(Random),
        race_selected(Random) {
    }
};

//! Information about a replay file.
struct ReplayInfo {
    float duration;
    unsigned int duration_gameloops;
    int32_t num_players;
    uint32_t data_build;
    uint32_t base_build;
    std::string map_name;
    std::string map_path;
    std::string replay_path;
    std::string version;
    ReplayPlayerInfo players[max_num_players];

    ReplayInfo() :
        duration(0.0f),
        duration_gameloops(0),
        num_players(0),
        data_build(0),
        base_build(0) {
    }

    bool GetPlayerInfo(ReplayPlayerInfo& replay_player_info, int playerID) const {
        for (int i = 0; i < num_players; ++i) {
            if (playerID == players[i].player_id) {
                replay_player_info = players[i];
                return true;
            }
        }

        return false;
    }

    float GetGameloopsPerSecond() const {
        return float(duration_gameloops) / duration;
    }
};

//! Power source information for Protoss.
struct PowerSource {
    PowerSource(const Point2D in_position, float in_radius, Tag in_tag) :
        position(in_position),
        radius(in_radius),
        tag(in_tag) {};

    //! Power source position.
    Point2D position;
    //! Power source radius.
    float radius;
    //! Unit tag of the power source.
    Tag tag;
};

}
