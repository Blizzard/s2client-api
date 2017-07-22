/*! \file sc2_score.h
    \brief Scores accumulated in a game as calculated by the game engine.
*/
#pragma once

#include "sc2_proto_interface.h"
#include "sc2_gametypes.h"
#include <vector>
#include <string>
#include <stdint.h>

namespace sc2 {

//! Source of a score.
enum class ScoreType {
    //! Map generated score (from curriculum maps with special scoring).
    Curriculum = 1,
    //! Summation of in-progress and current units/buildings value + minerals + vespene.
    Melee = 2
};

struct ScoreEntry {
    std::string name;
    int offset;
    bool use;
    bool nonzero;

    ScoreEntry() :
        offset(0),
        use(true),
        nonzero(false) {
    }
};

#define SET_ENTRY_BASE(STRUCTNAME, ENTRYNAME)                                       \
    {                                                                               \
        ScoreEntry new_entry;                                                       \
        new_entry.offset += (int)int64_t(&(((STRUCTNAME*)0)->ENTRYNAME));           \
        new_entry.name += #STRUCTNAME"."#ENTRYNAME;                                 \
        entries.push_back(new_entry);                                               \
    }

#define SET_ENTRY_BASE_STRUCT(STRUCTNAME, ENTRYNAME)                                \
    {                                                                               \
        ScoreEntry base_entry;                                                      \
        base_entry.offset += (int)int64_t(&(((STRUCTNAME*)0)->ENTRYNAME));          \
        base_entry.name += #STRUCTNAME"."#ENTRYNAME;                                \
        ((STRUCTNAME*)0)->ENTRYNAME.AddEntries(base_entry, entries);                \
    }

#define SET_ENTRY(STRUCTNAME, ENTRYNAME)                                            \
    {                                                                               \
        ScoreEntry new_entry = base;                                                \
        new_entry.offset += (int)int64_t(&(((STRUCTNAME*)0)->ENTRYNAME));           \
        new_entry.name += "."#ENTRYNAME;                                            \
        entries.push_back(new_entry);                                               \
    }

#define SET_ENTRY_STRUCT(STRUCTNAME, ENTRYNAME)                                     \
    {                                                                               \
        ScoreEntry new_entry = base;                                                \
        new_entry.offset += (int)int64_t(&(((STRUCTNAME*)0)->ENTRYNAME));           \
        new_entry.name += "."#ENTRYNAME;                                            \
        ((STRUCTNAME*)0)->ENTRYNAME.AddEntries(new_entry, entries);                 \
    }

//! Score by category.
struct CategoryScoreDetails {
    float none;
    float army;
    float economy;
    float technology;
    float upgrade;

    CategoryScoreDetails();

    static void AddEntries(ScoreEntry base, std::vector<ScoreEntry>& entries) {
        SET_ENTRY(CategoryScoreDetails, none)
        SET_ENTRY(CategoryScoreDetails, army)
        SET_ENTRY(CategoryScoreDetails, economy)
        SET_ENTRY(CategoryScoreDetails, technology)
        SET_ENTRY(CategoryScoreDetails, upgrade)
    }
};

//! Score for vitals.
struct VitalScoreDetails {
    float life;
    float shields;
    float energy;

    VitalScoreDetails();

    static void AddEntries(ScoreEntry base, std::vector<ScoreEntry>& entries) {
        SET_ENTRY(VitalScoreDetails, life)
        SET_ENTRY(VitalScoreDetails, shields)
        SET_ENTRY(VitalScoreDetails, energy)
    }
};

//! Detailed scores.
struct ScoreDetails {
    float idle_production_time;
    float idle_worker_time;

    float total_value_units;
    float total_value_structures;

    // Note the "killed_value" fields are a combination of minerals, vespene and a human designer guess. Maybe useful as a delta.
    // The weighting of the combination and the human designer guess is different (not symmetric) with the "total_value" fields!
    float killed_value_units;
    float killed_value_structures;

    float collected_minerals;
    float collected_vespene;

    float collection_rate_minerals;
    float collection_rate_vespene;

    float spent_minerals;
    float spent_vespene;

    CategoryScoreDetails food_used;

    CategoryScoreDetails killed_minerals;
    CategoryScoreDetails killed_vespene;

    CategoryScoreDetails lost_minerals;
    CategoryScoreDetails lost_vespene;

    CategoryScoreDetails friendly_fire_minerals;
    CategoryScoreDetails friendly_fire_vespene;

    CategoryScoreDetails used_minerals;
    CategoryScoreDetails used_vespene;

    CategoryScoreDetails total_used_minerals;
    CategoryScoreDetails total_used_vespene;

    VitalScoreDetails total_damage_dealt;
    VitalScoreDetails total_damage_taken;
    VitalScoreDetails total_healed;

    ScoreDetails();

    static void AddEntries(ScoreEntry base, std::vector<ScoreEntry>& entries) {
        SET_ENTRY(ScoreDetails, idle_production_time)
        SET_ENTRY(ScoreDetails, idle_worker_time)
        SET_ENTRY(ScoreDetails, total_value_units)
        SET_ENTRY(ScoreDetails, total_value_structures)
        SET_ENTRY(ScoreDetails, killed_value_units)
        SET_ENTRY(ScoreDetails, killed_value_structures)
        SET_ENTRY(ScoreDetails, collected_minerals)
        SET_ENTRY(ScoreDetails, collected_vespene)
        SET_ENTRY(ScoreDetails, collection_rate_minerals)
        SET_ENTRY(ScoreDetails, collection_rate_vespene)
        SET_ENTRY(ScoreDetails, spent_minerals)
        SET_ENTRY(ScoreDetails, spent_vespene)

        SET_ENTRY_STRUCT(ScoreDetails, food_used)
        SET_ENTRY_STRUCT(ScoreDetails, killed_minerals)
        SET_ENTRY_STRUCT(ScoreDetails, killed_vespene)
        SET_ENTRY_STRUCT(ScoreDetails, lost_minerals)
        SET_ENTRY_STRUCT(ScoreDetails, lost_vespene)
        SET_ENTRY_STRUCT(ScoreDetails, friendly_fire_minerals)
        SET_ENTRY_STRUCT(ScoreDetails, friendly_fire_vespene)
        SET_ENTRY_STRUCT(ScoreDetails, used_minerals)
        SET_ENTRY_STRUCT(ScoreDetails, used_vespene)
        SET_ENTRY_STRUCT(ScoreDetails, total_used_minerals)
        SET_ENTRY_STRUCT(ScoreDetails, total_used_vespene)

        SET_ENTRY_STRUCT(ScoreDetails, total_damage_dealt)
        SET_ENTRY_STRUCT(ScoreDetails, total_damage_taken)
        SET_ENTRY_STRUCT(ScoreDetails, total_healed)
    }
};

//! Scores.
struct Score {
    ScoreType score_type;
    float score;                     // Note: check score_type to know whether this is a melee score or curriculum score
    ScoreDetails score_details;

    // Access as a flat list of floats.
    static const int float_count_ = sizeof(ScoreDetails) / sizeof(float) + 1;
    const float* RawFloats() const { return &score; }

    Score();

    static void AddEntries(std::vector<ScoreEntry>& entries) {
        SET_ENTRY_BASE(Score, score)
        SET_ENTRY_BASE_STRUCT(Score, score_details)
    }

    bool IsEqual(const Score& other_score) const;
};

}
