#include "sc2utils/sc2_arg_parser.h"

#include <iostream>
#include <cstring>

namespace sc2 {

ArgParser::ArgParser() :
    options_(),
    usage_(),
    description_(),
    example_(),
    executable_name_() {
}

ArgParser::ArgParser(const std::string & executable_name) :
    options_(),
    usage_(),
    description_(),
    example_(),
    executable_name_(executable_name) {
}

ArgParser::ArgParser(const std::string& usage, const std::string& description, const std::string& example) :
    options_(),
    usage_(usage),
    description_(description),
    example_(example),
    executable_name_() {
}

void ArgParser::AddOptions(const std::vector<Arg>& options) {
    for (const auto& o : options) {
        options_.push_back(o);
        abbv_to_full_[o.abbreviation_] = o.fullname_;
    }
}

bool ArgParser::Parse(int argc, char* argv[]) {
    executable_name_ = argv[0];

    for (int i = 0; i < argc; ++i) {
        // Skip values.
        if (argv[i][0] != '-') {
            continue;
        }

        // If help is sent print out help and return false, don't parse.
        if (strcmp(argv[i], "--help") == 0) {
            PrintHelp();
            return false;
        }

        // Check that it's a valid option.
        bool valid_option = false;
        for (const auto& o : options_) {
            if (o.abbreviation_ == argv[i] || o.fullname_ == argv[i]) {
                valid_option = true;
                break;
            }
        }

        if (!valid_option) {
            std::cout << argv[i] << " is an unrecognized argument." << std::endl;
            return false;
        }

        // Determine if the arg is a fullname or abbrevation.
        std::string fullname = argv[i];
        // Get the fullname if it's an abbreviation.
        if (fullname[1] != '-') {
            fullname = abbv_to_full_[fullname];
        }

        std::string value;
        if (i < argc - 1) {
            if (argv[i + 1][0] != '-') {
                value = argv[i + 1];
            }
        }

        // Remove the dashes.
        fullname.erase(0, 2);
        full_to_value_[fullname] = value;
    }

    // Iterate the options and verify we have all necessary arguments.
    for (const auto& o : options_) {
        if (o.required_) {
            std::string fullname = o.fullname_;
            fullname.erase(0, 2);
            std::unordered_map<std::string, std::string>::const_iterator f =
                full_to_value_.find(fullname);
            if (f == full_to_value_.end()) {
                // This argument can be supplied in multiple ways.
                return false;
            }
        }
    }

    return true;
}

bool ArgParser::Get(const std::string& identifier, std::string& value) {
    std::string fullname = identifier;
    // If the identifier is the abbrevation turn it into the fullname
    if (fullname.size() == 1) {
        std::unordered_map<std::string, std::string>::const_iterator f =
            abbv_to_full_.find(std::string("-") + identifier);

        if (f == abbv_to_full_.end()) {
            return false;
        }

        fullname = f->second;
    }

    if (fullname[0] == '-') {
        fullname.erase(0, 2);
    }

    std::unordered_map<std::string, std::string>::const_iterator f =
        full_to_value_.find(fullname);

    if (f == full_to_value_.end()) {
        return false;
    }

    value = f->second;

    return true;
}

void ArgParser::PrintHelp() {
    PrintUsage();
    std::cout << "Options -" << std::endl;
    for (const auto& o : options_) {
        std::cout << "  " << o.abbreviation_ << ", " << o.fullname_ << " " << o.description_ << std::endl;
    }
}

void ArgParser::PrintUsage() {
    std::cout << "Usage: " << executable_name_ << " ";
    // Append required arguments.
    for (const auto& o : options_) {
        if (o.required_) {
            std::string fullname = o.fullname_;
            fullname.erase(0, 2);
            std::cout << o.abbreviation_ << " [" << fullname << "] ";
        }
    }
    std::cout << std::endl << std::endl;
}

}
