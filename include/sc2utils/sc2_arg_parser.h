#pragma once

#include <vector>
#include <string>
#include <unordered_map>

namespace sc2 {

struct Arg {
    std::string abbreviation_;
    std::string fullname_;
    std::string description_;
    bool required_;
};

class ArgParser {
public:
    ArgParser();
    ArgParser(const std::string& executable_name);
    ArgParser(const std::string& usage, const std::string& description, const std::string& example="");
    

    void AddOptions(const std::vector<Arg>& options);
    bool Parse(int argc, char* argv[]);

    // If the arg exists returns true and if a value exists for it fill it.
    bool Get(const std::string& identifier, std::string& value);
    void PrintHelp();
    void PrintUsage();

private:
    std::vector<Arg> options_;
    std::unordered_map<std::string, std::string> abbv_to_full_;
    std::unordered_map<std::string, std::string> full_to_value_;

    std::string usage_;
    std::string description_;
    std::string example_;
    std::string executable_name_;
};

}
