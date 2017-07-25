#include "sc2api/sc2_api.h"

#include "sc2utils/sc2_manage_process.h"
#include "sc2utils/sc2_simple_serialization.h"

#include <algorithm>


static const char* ReplayListFile = "replaylist.txt";
static const char* GatheredDataFile = "gathereddata.txt";

void AddSpacesToColumn(std::string& str, int spaces = 60) {
    for (int i = int(str.length()); i < spaces; ++i)
        str += " ";
}

bool FirstWordOnly(std::string& str_out, const std::string& str_in) {
    if (str_in.length() < 2)
        return false;

    for (int i = 0; i < str_in.length(); ++i) {
        if (str_in[i] == ' ') {
            str_out.assign(str_in, 0, i);
            return true;
        }
    }

    return false;
}

class RaceEvent {
public:
    bool terran_;
    bool zerg_;
    bool protoss_;
    bool neutral_;

    RaceEvent() :
        terran_(false),
        zerg_(false),
        protoss_(false),
        neutral_(false) {
    }

    template<class Stream> void Serialize(Stream& s) {
        sc2::SerializeT(s, terran_);
        sc2::SerializeT(s, zerg_);
        sc2::SerializeT(s, protoss_);
        sc2::SerializeT(s, neutral_);
    }

    std::string RaceName() const {
        if (terran_)
            return "TERRAN";
        if (zerg_)
            return "ZERG";
        if (protoss_)
            return "PROTOSS";

        return "NEUTRAL";
    }

    void AddRace(sc2::ReplayObserver* replay_observer, int player_id = -1) {
        if (IsRaceSet())
            return;

        const sc2::ReplayInfo& replay_info = replay_observer->ReplayControl()->GetReplayInfo();
        if (player_id < 0)
            player_id = replay_observer->Observation()->GetPlayerID();

        if (player_id == 0 || player_id == sc2::max_num_players) {
            neutral_ = true;
            return;
        }
        assert(player_id >= 0 && player_id < sc2::max_num_players);

        sc2::ReplayPlayerInfo replay_player_info;
        if (replay_info.GetPlayerInfo(replay_player_info, player_id)) {
            switch (replay_player_info.race)
            {
                case sc2::Terran:
                    terran_ = true;
                    break;
                case sc2::Zerg:
                    zerg_ = true;
                    break;
                case sc2::Protoss:
                    protoss_ = true;
                    break;
                default:
                    assert(0);
                    break;
            }
        }
        else {
            neutral_ = true;
        }
    }

    bool IsOneRace() const {
        int count = 0;
        if (terran_)
            ++count;
        if (zerg_)
            ++count;
        if (protoss_)
            ++count;

        return count < 2;
    }

    bool IsRaceSet() const {
        return terran_ || zerg_ || protoss_ || neutral_;
    }
};

class AbilityEvent : public RaceEvent {
public:
    // AbilityData.
    sc2::AbilityID ability_id_;
    std::string link_name_;
    uint32_t link_index_;
    std::string button_name_;
    std::string friendly_name_;

    int count_;
    std::string name_;
    std::string line_;

    bool target_none_;
    bool target_unit_;
    bool target_point_;

    AbilityEvent() :
        count_(0),
        target_none_(false),
        target_unit_(false),
        target_point_(false),
        RaceEvent() {
    }

    template<class Stream> void Serialize(Stream& s) {
        RaceEvent::Serialize(s);
        sc2::SerializeT(s, ability_id_);
        sc2::SerializeT(s, link_name_);
        sc2::SerializeT(s, link_index_);
        sc2::SerializeT(s, button_name_);
        sc2::SerializeT(s, friendly_name_);
        sc2::SerializeT(s, count_);
        sc2::SerializeT(s, name_);
        sc2::SerializeT(s, line_);
        sc2::SerializeT(s, target_none_);
        sc2::SerializeT(s, target_unit_);
        sc2::SerializeT(s, target_point_);
    }

    std::string GetBestName() const {
        std::string name = button_name_;
        if (friendly_name_.length() > 1)
            name = friendly_name_;

        for (int i = 0; i < name.length(); ++i)
            if (name[i] == ' ')
                name[i] = '_';

        return name;
    }

    std::string BestName() const {
        std::string best_name = GetBestName();
        std::transform(best_name.begin(), best_name.end(), best_name.begin(), ::toupper);
        return best_name;
    }

    std::string Enum(bool disambiguate) {
        line_ = GetBestName();
        if (disambiguate) {
            // If there is a friendly name, use that to begin the line.
            FirstWordOnly(line_, friendly_name_);

            // Add an underscore if there is a word already in the line.
            if (line_.length() > 1) {
                line_ += "_";
            }

            // Assemble the line.
            line_ += link_name_ + "_" + std::to_string(link_index_);
        }
        std::transform(line_.begin(), line_.end(), line_.begin(), ::toupper);
        name_ = line_;
        AddSpacesToColumn(line_);
        line_ += " = " + ability_id_.to_string() + ",";
        AddSpacesToColumn(line_, 70);
        line_ += "// Target: ";
        if (target_none_)
            line_ += "None";
        if (target_unit_) {
            if (target_none_)
                line_ += ", ";
            line_ += "Unit";
        }
        if (target_point_) {
            if (target_none_ || target_unit_)
                line_ += ", ";
            line_ += "Point";
        }
        line_ += ".";

        return line_;
    }
};

class UnitEvent : public RaceEvent {
public:
    int count_;
    std::string name_;
    sc2::UnitTypeID unit_type_id_;
    std::set<uint32_t> abilities_;
    std::set<uint32_t> possible_abilities_;

    UnitEvent() :
        count_(0),
        RaceEvent() {
    }

    template<class Stream> void Serialize(Stream& s) {
        RaceEvent::Serialize(s);

        sc2::SerializeT(s, count_);
        sc2::SerializeT(s, name_);
        sc2::SerializeT(s, unit_type_id_);
        sc2::SerializeT(s, abilities_);
        sc2::SerializeT(s, possible_abilities_);
    }

    std::string Enum(const std::vector<AbilityEvent>& abilities) const {
        std::string line = RaceName() + "_" + name_;
        std::transform(line.begin(), line.end(), line.begin(), ::toupper);
        AddSpacesToColumn(line);
        line += " = " + unit_type_id_.to_string() + ",";
        if (abilities_.size() > 0) {
            AddSpacesToColumn(line, 70);
            line += "// ";
            bool add_comma = false;
            for (const auto& ability_id : abilities_) {
                if (ability_id >= abilities.size())
                    continue;

                const AbilityEvent& ability = abilities[ability_id];
                if (ability.count_ < 1)
                    continue;

                if (add_comma)
                    line += ", ";
                line += ability.name_;
                add_comma = true;
            }
        }
        return line;
    }
};

std::string FormatEnumLine(int enum_value, std::string enum_str) {
    std::transform(enum_str.begin(), enum_str.end(), enum_str.begin(), ::toupper);
    AddSpacesToColumn(enum_str);
    enum_str += " = " + std::to_string(enum_value) + ",";
    return enum_str;
}

class GatherEvents {
public:
    std::mutex mu_ability_;
    std::mutex mu_unit_;
    std::mutex mu_upgrade_;
    std::mutex mu_buff_;
    std::vector<AbilityEvent> abilities_;
    std::vector<UnitEvent> units_;
    std::vector<std::string> upgrades_;
    std::vector<std::string> buffs_;
    sc2::Abilities abilities_data_;

    template<class Stream> void SerializeVector(Stream& s, std::vector<std::string>& v) {
        int count = static_cast<int>(v.size());
        sc2::SerializeT(s, count);
        v.resize(count);
        for (std::string& t : v)
            sc2::SerializeT(s, t);
    }

    template<class Stream, typename T> void SerializeVector(Stream& s, std::vector<T>& v) {
        int count = static_cast<int>(v.size());
        sc2::SerializeT(s, count);
        v.resize(count);
        for (T& t : v)
            t.Serialize(s);
    }

    template<class Stream> void Serialize(Stream& s) {
        SerializeVector(s, abilities_);
        SerializeVector(s, units_);
        SerializeVector(s, upgrades_);
        SerializeVector(s, buffs_);
    }

    void Load(const std::string& file_path) {
        if (!sc2::DoesFileExist(file_path))
            return;

        std::ifstream data_file(file_path);
        Serialize(data_file);
    }

    void Save(const std::string& file_path) {
        std::ofstream data_file(file_path, std::ofstream::out | std::ofstream::trunc);
        Serialize(data_file);
    }

    void AddAllAbilities(const sc2::Abilities& abilities) {
        std::lock_guard<std::mutex> lock(mu_ability_);

        if (abilities_data_.size() < 1) {
            abilities_data_ = abilities;
        }

        if (abilities.size() > abilities_.size())
            abilities_.resize(abilities.size());

        for (int i = 0; i < abilities_.size(); ++i) {
            assert(abilities[i].ability_id == i);
            abilities_[i].ability_id_ = abilities[i].ability_id;
            abilities_[i].link_name_ = abilities[i].link_name;
            abilities_[i].link_index_ = abilities[i].link_index;
            abilities_[i].button_name_ = abilities[i].button_name;
            abilities_[i].friendly_name_ = abilities[i].friendly_name;
            switch (abilities[i].target) {
            case sc2::AbilityData::Target::None:
                abilities_[i].target_none_ = true;
                break;
            case sc2::AbilityData::Target::Point:
                abilities_[i].target_point_= true;
                break;
            case sc2::AbilityData::Target::Unit:
                abilities_[i].target_unit_= true;
                break;
            case sc2::AbilityData::Target::PointOrUnit:
                abilities_[i].target_point_= true;
                abilities_[i].target_unit_= true;
                break;
            }
            if (abilities[i].is_instant_placement) {
                abilities_[i].target_none_ = true;
            }
        }
    }

    void AddAbilityEvent(int ability_id, sc2::ReplayObserver* replay_observer) {
        std::lock_guard<std::mutex> lock(mu_ability_);

        // Grow the array if needed.
        if (ability_id >= abilities_.size())
            abilities_.resize(ability_id + 1);

        AbilityEvent& event = abilities_[ability_id];
        ++event.count_;
        event.AddRace(replay_observer);
        abilities_[ability_id] = event;
    }

    void AddUnitEvent(int unit_id, int player_id, sc2::ReplayObserver* replay_observer) {
        std::lock_guard<std::mutex> lock(mu_unit_);

        if (unit_id == 0)
            return;

        // Grow the array if needed.
        if (unit_id >= units_.size())
            units_.resize(unit_id + 1);

        UnitEvent& event = units_[unit_id];

        if (event.count_ == 0) {
            const sc2::UnitTypes& unit_types = replay_observer->Observation()->GetUnitTypeData();
            assert(unit_id < unit_types.size());
            if (unit_id >= unit_types.size())
                return;

            event.name_ = unit_types[unit_id].name;
            event.unit_type_id_ = unit_types[unit_id].unit_type_id;
        }
        ++event.count_;
        event.AddRace(replay_observer, player_id);
        units_[unit_id] = event;
    }

    void AddUnitAbilityEvent(int unit_id, sc2::AbilityID ability_id) {
        std::lock_guard<std::mutex> lock(mu_unit_);
        if (ability_id == sc2::AbilityID(0) || unit_id >= units_.size())
            return;

        AbilityEvent& ability_event = abilities_[ability_id];
        if (ability_event.count_ == 0)
            ++ability_event.count_;

        UnitEvent& event = units_[unit_id];
        if (event.possible_abilities_.find(ability_id) != event.possible_abilities_.end())
            event.abilities_.insert(ability_id);
    }

    void AddUnitPossibleAbility(int unit_id, sc2::AbilityID ability_id) {
        std::lock_guard<std::mutex> lock(mu_unit_);
        if (unit_id >= units_.size())
            return;

        UnitEvent& event = units_[unit_id];
        event.possible_abilities_.insert(ability_id);
    }

    bool IsAbilityNameUnique(int ability_id) const {
        assert(ability_id >= 0 && ability_id < abilities_.size());

        const AbilityEvent& ability = abilities_[ability_id];
        assert(ability.count_ > 0);

        std::string button_name = ability.BestName();
        if (button_name.length() < 2)
            return false;

        bool found = false;
        for (const AbilityEvent& other_event : abilities_) {
            if (other_event.count_ == 0)
                continue;

            std::string other_button_name = other_event.BestName();
            if (other_button_name.length() < 2)
                continue;

            if (button_name.compare(other_button_name) == 0) {
                if (found)
                    return false;
                found = true;
            }
        }

        return true;
    }

    void AddUpgradeEvent(uint32_t upgrade, sc2::ReplayObserver* replay_observer) {
        std::lock_guard<std::mutex> lock(mu_upgrade_);

        const sc2::Upgrades& upgrades = replay_observer->Observation()->GetUpgradeData();
        assert(upgrade < upgrades.size());
        if (upgrade >= upgrades.size())
            return;

        if (upgrades_.size() < upgrades.size())
            upgrades_.resize(upgrades.size());

        upgrades_[upgrade] = upgrades[upgrade].name;
    }

    void AddBuffEvent(uint32_t buff, sc2::ReplayObserver* replay_observer) {
        std::lock_guard<std::mutex> lock(mu_buff_);

        const sc2::Buffs& buffs = replay_observer->Observation()->GetBuffData();
        assert(buff < buffs.size());
        if (buff >= buffs.size())
            return;

        if (buffs_.size() < buffs.size())
            buffs_.resize(buffs.size());

        buffs_[buff] = buffs[buff].name;
    }

    void BucketUnits(
        std::vector<std::string>& terran,
        std::vector<std::string>& terran_noninteractive,
        std::vector<std::string>& zerg,
        std::vector<std::string>& zerg_noninteractive,
        std::vector<std::string>& protoss,
        std::vector<std::string>& protoss_noninteractive,
        std::vector<std::string>& neutral
    ) {
        for (const UnitEvent& event : units_) {
            if (event.count_ < 1)
                continue;
            std::string str = event.Enum(abilities_);

            if (event.terran_) {
                if (event.abilities_.size() > 0)
                    terran.push_back(str);
                else
                    terran_noninteractive.push_back(str);
                continue;
            }

            if (event.protoss_) {
                if (event.abilities_.size() > 0)
                    protoss.push_back(str);
                else
                    protoss_noninteractive.push_back(str);
                continue;
            }

            if (event.zerg_) {
                if (event.abilities_.size() > 0)
                    zerg.push_back(str);
                else
                    zerg_noninteractive.push_back(str);
                continue;
            }

            neutral.push_back(str);
        }

        std::sort(terran.begin(), terran.end());
        std::sort(terran_noninteractive.begin(), terran_noninteractive.end());
        std::sort(zerg.begin(), zerg.end());
        std::sort(zerg_noninteractive.begin(), zerg_noninteractive.end());
        std::sort(protoss.begin(), protoss.end());
        std::sort(protoss_noninteractive.begin(), protoss_noninteractive.end());
        std::sort(neutral.begin(), neutral.end());
    }

    void SetNamesForAbilities() {
        // TODO: The count for ability zero should never occur. This is rare, but needs investigation.
        abilities_[0].count_ = 0;
        for (AbilityEvent& event : abilities_) {
            if (event.count_ > 0)
                event.Enum(!IsAbilityNameUnique(event.ability_id_));
        }
    }

    void WriteHeaderFileComments(std::ofstream& file) {
        file << "/*! \\file sc2_typeenums.h" << std::endl;
        file << "    \\brief A list of enums provided for your convenience." << std::endl;
        file << "" << std::endl;
        file << "All units and abilities are represented as unsigned numbers in the StarCraft II engine. This can make issue abilities unnecessarily hard." << std::endl;
        file << "This file aims to provide a type safe and named way of representing unit types and ability types. Entries in it are generated by a custom" << std::endl;
        file << "built script so it is possible they don't have every possible id in the engine but they should have most of the common ones." << std::endl;
        file << "See stableid.json and stableabilityid.json to see all of them." << std::endl;
        file << "*/" << std::endl;
        file << std::endl;
    }

    void WriteHeaderFile() {
        std::ofstream file("sc2_typeenums.h", std::ofstream::trunc);

        WriteHeaderFileComments(file);

        file << "#pragma once" << std::endl << std::endl;
        file << "#include \"sc2_types.h\"" << std::endl << std::endl;
        file << "namespace sc2 {" << std::endl << std::endl;

        file << "enum class UNIT_TYPEID;" << std::endl;
        file << "enum class ABILITY_ID;" << std::endl;
        file << "enum class UPGRADE_ID;" << std::endl;
        file << "enum class BUFF_ID;" << std::endl;

        file << "typedef SC2Type<UNIT_TYPEID>  UnitTypeID;" << std::endl;
        file << "typedef SC2Type<ABILITY_ID>   AbilityID;" << std::endl;
        file << "typedef SC2Type<UPGRADE_ID>   UpgradeID;" << std::endl;
        file << "typedef SC2Type<BUFF_ID>      BuffID;" << std::endl;

        file << std::endl;

        // Units.
        {
            std::vector<std::string> terran;
            std::vector<std::string> terran_noninteractive;
            std::vector<std::string> zerg;
            std::vector<std::string> zerg_noninteractive;
            std::vector<std::string> protoss;
            std::vector<std::string> protoss_noninteractive;
            std::vector<std::string> neutral;
            BucketUnits(terran, terran_noninteractive, zerg, zerg_noninteractive, protoss, protoss_noninteractive, neutral);

            file << "enum class UNIT_TYPEID {" << std::endl;

            file << "    // Terran" << std::endl;
            for (const auto& str : terran) {
                file << "    " << str << std::endl;
            }

            file << std::endl << "    // Terran non-interactive" << std::endl;
            for (const auto& str : terran_noninteractive) {
                file << "    " << str << std::endl;
            }

            file << std::endl << "    // Zerg" << std::endl;
            for (const auto& str : zerg) {
                file << "    " << str << std::endl;
            }

            file << std::endl << "    // Zerg non-interactive" << std::endl;
            for (const auto& str : zerg_noninteractive) {
                file << "    " << str << std::endl;
            }

            file << std::endl << "    // Protoss" << std::endl;
            for (const auto& str : protoss) {
                file << "    " << str << std::endl;
            }

            file << std::endl << "    // Protoss non-interactive" << std::endl;
            for (const auto& str : protoss_noninteractive) {
                file << "    " << str << std::endl;
            }

            file << std::endl << "    // Neutral" << std::endl;
            for (const auto& str : neutral) {
                file << "    " << str << std::endl;
            }

            // Close for 'enum class UNIT_TYPEID {'.
            file << "};" << std::endl << std::endl;
        }

        // Abilities.
        {
            file << "enum class ABILITY_ID {" << std::endl;

            std::vector<std::string> abils;
            for (const AbilityEvent& event : abilities_) {
                if (event.count_ > 0)
                    abils.push_back(event.line_);
            }

            // Put the smart command at the beginning.
            file << "    " << abils[0] << std::endl << std::endl;
            abils.erase(abils.begin());

            std::sort(abils.begin(), abils.end());

            for (const auto& str : abils)
                file << "    " << str << std::endl;

            // Close for 'enum class ABILITY_ID {'.
            file << "};" << std::endl << std::endl;
        }

        // Upgrades.
        {
            file << "enum class UPGRADE_ID {" << std::endl;

            for (int i = 0; i < upgrades_.size(); ++i) {
                const std::string& str = upgrades_[i];
                if (str.length() < 2)
                    continue;

                std::string formatted_str = FormatEnumLine(i, str);
                file << "    " << formatted_str << std::endl;
            }

            // Close for 'enum class UPGRADE_ID {'.
            file << "};" << std::endl << std::endl;
        }

        // Buffs.
        {
            file << "enum class BUFF_ID {" << std::endl;

            for (int i = 0; i < buffs_.size(); ++i) {
                const std::string& str = buffs_[i];
                if (str.length() < 2)
                    continue;

                std::string formatted_str = FormatEnumLine(i, str);
                file << "    " << formatted_str << std::endl;
            }

            // Close for 'enum class BUFF_ID {'.
            file << "};" << std::endl << std::endl;
        }

        // Typedefs and function prototypes.
        file << "//! Converts a UNIT_TYPEID into a string of the same name." << std::endl;
        file << "const char* UnitTypeToName(UnitTypeID unit_type);" << std::endl << std::endl;
        file << "//! Converts a ABILITY_ID into a string of the same name." << std::endl;
        file << "const char* AbilityTypeToName(AbilityID ability_type);" << std::endl << std::endl;
        file << "//! Converts a UPGRADE_ID into a string of the same name." << std::endl;
        file << "const char* UpgradeIDToName(UpgradeID upgrade_id);" << std::endl << std::endl;
        file << "//! Converts a BUFF_ID into a string of the same name." << std::endl;
        file << "const char* BuffIDToName(BuffID buff_id);" << std::endl << std::endl;

        // Close for 'namespace sc2 {'.
        file << "}" << std::endl << std::endl;
        file.close();
    }

    void WriteCCFile() {
        std::ofstream file("sc2_typeenums.cc", std::ofstream::trunc);

        file << "#include \"sc2api/sc2_typeenums.h\"" << std::endl << std::endl;
        file << "namespace sc2 {" << std::endl << std::endl;

        // Units.
        {
            file << "const char* UnitTypeToName(UnitTypeID unit_type) {" << std::endl;
            file << "    switch ((UNIT_TYPEID)unit_type) {" << std::endl;

            for (const UnitEvent& event : units_) {
                if (event.count_ < 1)
                    continue;
                std::string name = event.RaceName() + "_" + event.name_;
                std::transform(name.begin(), name.end(), name.begin(), ::toupper);
                std::string str = "        case UNIT_TYPEID::" + name + ": ";
                AddSpacesToColumn(str);
                file << str << "return " << "\"" << name << "\"; // " << std::to_string(event.count_) << std::endl;
            }

            file << "    }" << std::endl << std::endl;
            file << "    return \"UNKNOWN\";" << std::endl;
            file << "};" << std::endl << std::endl;
        }

        // Abilities.
        {
            file << "const char* AbilityTypeToName(AbilityID ability_type) {" << std::endl;
            file << "    switch ((ABILITY_ID)ability_type) {" << std::endl;

            for (const AbilityEvent& event : abilities_) {
                if (event.count_ < 1)
                    continue;
                std::string name =  event.name_;
                std::transform(name.begin(), name.end(), name.begin(), ::toupper);
                std::string str = "        case ABILITY_ID::" + name + ": ";
                AddSpacesToColumn(str);
                file << str << "return " << "\"" << name << "\"; // " << std::to_string(event.count_) << std::endl;
            }

            file << "    }" << std::endl << std::endl;
            file << "    return \"UNKNOWN\";" << std::endl;
            file << "};" << std::endl << std::endl;
        }

        // Upgrades.
        {
            file << "const char* UpgradeIDToName(UpgradeID upgrade_id) {" << std::endl;
            file << "    switch ((UPGRADE_ID)upgrade_id) {" << std::endl;

            for (int i = 0; i < upgrades_.size(); ++i) {
                std::string name = upgrades_[i];
                if (name.length() < 2)
                    continue;

                std::transform(name.begin(), name.end(), name.begin(), ::toupper);
                std::string str = "        case UPGRADE_ID::" + name + ": ";
                AddSpacesToColumn(str);
                file << str << "return " << "\"" << name << "\";" << std::endl;
            }

            file << "    }" << std::endl << std::endl;
            file << "    return \"UNKNOWN\";" << std::endl;
            file << "};" << std::endl << std::endl;
        }

        // Buffs.
        {
            file << "const char* BuffIDToName(BuffID buff_id) {" << std::endl;
            file << "    switch ((BUFF_ID)buff_id) {" << std::endl;

            for (int i = 0; i < buffs_.size(); ++i) {
                std::string name = buffs_[i];
                if (name.length() < 2)
                    continue;

                std::transform(name.begin(), name.end(), name.begin(), ::toupper);
                std::string str = "        case BUFF_ID::" + name + ": ";
                AddSpacesToColumn(str);
                file << str << "return " << "\"" << name << "\";" << std::endl;
            }

            file << "    }" << std::endl << std::endl;
            file << "    return \"UNKNOWN\";" << std::endl;
            file << "};" << std::endl << std::endl;
        }

        // Close for 'namespace sc2 {'.
        file << "}" << std::endl << std::endl;
        file.close();
    }

    static void WriteAbilityData(std::ofstream& file, const sc2::AbilityData& ability) {
        file << std::to_string(ability.ability_id) << " (";
        if (ability.friendly_name.c_str() && *ability.friendly_name.c_str()) {
            file << ability.friendly_name << ", ";
        }
        file << ability.link_name <<  ", " << std::to_string(ability.link_index) << ")" << std::endl;
    }

    void WriteRemapsLog() {
        std::ofstream file("remapped_abilities.txt", std::ofstream::trunc);

        file << "------------------------------------------------------------------------" << std::endl;
        file << "Abilities remapped:" << std::endl;
        file << "------------------------------------------------------------------------" << std::endl << std::endl;

        for (int i = 0; i < abilities_data_.size(); ++i) {
            const sc2::AbilityData& ability = abilities_data_[i];
            if (ability.remaps_from_ability_id.size() < 1)
                continue;

            file << "------------------------------------------------------------------------" << std::endl;
            WriteAbilityData(file, ability);

            for (int j = 0; j < ability.remaps_from_ability_id.size(); ++j) {
                file << "    ";
                WriteAbilityData(file, abilities_data_[ability.remaps_from_ability_id[j]]);
            }
        }

        file.close();
    }

    void WriteAvailabilityLog() {
        std::ofstream file("available_abilities.txt", std::ofstream::trunc);

        for (int i = 0; i < abilities_data_.size(); ++i) {
            const sc2::AbilityData& ability = abilities_data_[i];
            if (ability.available) {
                file << "y ";
            }
            else {
                file << "n ";
            }
            WriteAbilityData(file, ability);
        }

        file.close();
    }
};

class Replay : public sc2::ReplayObserver {
public:
    GatherEvents& gather_events_;

    Replay(GatherEvents& gather_events) :
        gather_events_(gather_events),
        sc2::ReplayObserver() {
    }

    void ProcessObservation() {
        if (!Control()->IsInGame())
            return;

        const sc2::ObservationInterface* obs = Observation();
        sc2::QueryInterface* query = Query();

        // Player data.
        const std::vector<sc2::UpgradeID>& upgrades = obs->GetUpgrades();
        for (uint32_t upgrade_index = 0; upgrade_index < upgrades.size(); ++upgrade_index) {
            gather_events_.AddUpgradeEvent(upgrades[upgrade_index], this);
        }

        // Buffs.
        {
            const std::vector<sc2::Unit>& units = obs->GetUnits();
            for (const sc2::Unit& unit : units) {
                for (uint32_t buff_index = 0; buff_index < unit.buffs.size(); ++buff_index) {
                    gather_events_.AddBuffEvent(unit.buffs[buff_index], this);
                }
            }
        }

        // Unit abilities.
        {
            std::vector<sc2::Tag> unit_tags;
            const std::vector<sc2::Unit>& units = obs->GetUnits();
            for (const sc2::Unit& unit : units)
                unit_tags.push_back(unit.tag);

            std::vector<sc2::AvailableAbilities> abilities = query->GetAbilitiesForUnits(unit_tags);
            assert(abilities.size() == unit_tags.size());
            for (int i = 0; i < units.size(); ++i) {
                sc2::UnitTypeID unit_type = units[i].unit_type;
                const sc2::AvailableAbilities& abilities_for_unit = abilities[i];

                // The unit type not being set signifies that unit queries cannot be made on the unit.

                // The tags must match.
                assert(units[i].tag == abilities_for_unit.unit_tag);

                // The if the unit type is set, it must match the unit type in the current observation.
                // Note this may not be a valid test in real-time mode.
                assert(!abilities_for_unit.IsValid() || abilities_for_unit.unit_type_id == units[i].unit_type);

                // No abilities should be set unless the unit type is set.
                assert(abilities_for_unit.IsValid() || abilities_for_unit.abilities.size() == 0);

                for (const sc2::AvailableAbility& available_ability : abilities_for_unit.abilities) {
                    sc2::AbilityID possible_ability_id = available_ability.ability_id;
                    gather_events_.AddUnitPossibleAbility(unit_type, possible_ability_id);

                    // If the ability has a button name, assume it will be valid.
                    if (uint32_t(possible_ability_id) < gather_events_.abilities_.size()) {
                        const AbilityEvent& abil_event = gather_events_.abilities_[possible_ability_id];
                        if (abil_event.button_name_.length() > 1) {
                            gather_events_.AddUnitAbilityEvent(unit_type, possible_ability_id);
                        }
                    }
                }
            }
        }

        // Units.
        {
            const sc2::Units& units_added = obs->GetUnitsAdded();
            for (const sc2::Unit& unit : units_added) {
                gather_events_.AddUnitEvent(unit.unit_type, unit.owner, this);
            }
        }

        // Abilities.
        {
            const sc2::RawActions& actions = obs->GetRawActions();
            for (const sc2::ActionRaw& action : actions) {
                gather_events_.AddAbilityEvent(action.ability_id, this);

                // Apply the action to the unit types.
                for (int i = 0; i < action.unit_tags.size(); ++i) {
                    sc2::Tag unit_tag = action.unit_tags[0];
                    const sc2::Unit* unit = obs->GetUnit(unit_tag);
                    if (unit) {
                        gather_events_.AddUnitAbilityEvent(unit->unit_type, action.ability_id);
                    }
                }
            }
        }
    }

    void OnGameStart() final {
        gather_events_.AddAllAbilities(Observation()->GetAbilityData());
        ProcessObservation();
    }

    void OnStep() final {
        ProcessObservation();
    }

    void OnGameEnd() final {
    }
};


int main(int argc, char* argv[]) {
    sc2::Coordinator coordinator;
    if (!coordinator.LoadSettings(argc, argv)) {
        std::cout << "Unable to find or parse settings." << std::endl;
        return 1;
    }

    coordinator.SetMultithreaded(true);
    coordinator.SetStepSize(20);

    // Note that the LoadReplayList step will allow processing to be stopped and resumed.
    if (!coordinator.LoadReplayList(ReplayListFile)) {
        if (!coordinator.SetReplayPath("C:/folder2/")) {
            std::cout << "Unable to find replays." << std::endl;
            return 1;
        }
    }

    if (!coordinator.HasReplays()) {
        std::cout << "All replays have already been processed." << std::endl;
        std::cout << "If you wish to start again, delete the " << ReplayListFile << " file." << std::endl;
        return 0;
    }

    GatherEvents gather_events;

    gather_events.Load(GatheredDataFile);

    Replay obs1(gather_events);
    Replay obs2(gather_events);
    Replay obs3(gather_events);
    Replay obs4(gather_events);
    /*Replay obs5(gather_events);
    Replay obs6(gather_events);
    Replay obs7(gather_events);
    Replay obs8(gather_events);*/

    coordinator.AddReplayObserver(&obs1);
    coordinator.AddReplayObserver(&obs2);
    coordinator.AddReplayObserver(&obs3);
    coordinator.AddReplayObserver(&obs4);
    /*coordinator.AddReplayObserver(&obs5);
    coordinator.AddReplayObserver(&obs6);
    coordinator.AddReplayObserver(&obs7);
    coordinator.AddReplayObserver(&obs8);*/

    while (coordinator.Update()) {
        if (sc2::PollKeyPress()) {
            break;
        }
    }

    gather_events.Save(GatheredDataFile);
    coordinator.SaveReplayList(ReplayListFile);

    gather_events.WriteAvailabilityLog();
    gather_events.WriteRemapsLog();
    gather_events.SetNamesForAbilities();
    gather_events.WriteHeaderFile();
    gather_events.WriteCCFile();

    coordinator.WaitForAllResponses();
}