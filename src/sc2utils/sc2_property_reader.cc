#include "sc2utils/sc2_property_reader.h"

#include <fstream>
#include <iostream>

namespace sc2 {

PropertyReader::PropertyReader() :
    file_read_(false) {
}

PropertyReader::PropertyReader(const std::string& file_name) :
    file_read_(false) {
    LoadFile(file_name);
}

PropertyReader::~PropertyReader() {

}

bool PropertyReader::IsLoaded() const {
    return file_read_;
}

bool PropertyReader::LoadFile(const std::string& file_name) {
    file_read_ = false;
    properties_.clear();

    std::ifstream file(file_name);
    if (!file.is_open()) {
        return false;
    }    
    file_read_ = true;

    std::string line;
    while (std::getline(file, line)) {
        // If first character on line is # assume comment, if it's blank assume malformed/incorrect/empty line and skip it
        if (line[0] == '#' || line[0] == ' ' || line.empty() || line.length() < 2)
            continue;

        std::string key;
        std::string value;
        unsigned int index = 0;
        size_t equal_index = line.find_first_of('=');
        // Extract key
        for (; index < equal_index; ++index) {
            // Spaces are allowed in key except in the last character
            if (index == equal_index - 1 && line[index] == ' ')
                continue;
 
            key += line[index];
        }

        // Extract value
        for (index = (unsigned int) equal_index + 1; index < line.size(); ++index) {
            // Spaces are allowed in value except in the first character
            if (index == equal_index + 1 && line[index] == ' ')
                continue;

            value += line[index];
        }

        properties_[key] = value;
    }
    file.close();
    return true;
}

void PropertyReader::Free() {
    properties_.clear();
}

bool PropertyReader::Read(const std::string& key, std::function<void(const std::string& v)> convert) {
    if (properties_.empty()) {
        std::cerr << "No properties file loaded" << std::endl;
        return false;
    }

    ConstMapIterator it = properties_.find(key);
    if (it != properties_.end()) {
        convert(it->second);
        return true;
    }

    return false;
}

bool PropertyReader::ReadInt(const std::string& key, int& value) {
    auto to_int = [&value](const std::string& v) {
        value = std::stoi(v);
    };
    return Read(key, to_int);
}

bool PropertyReader::ReadFloat(const std::string& key, float& value) {
    auto to_float = [&value](const std::string& v) {
        value = std::stof(v);
    };
    return Read(key, to_float);
}

bool PropertyReader::ReadString(const std::string& key, std::string& value) {
    auto to_string = [&value](const std::string& v) {
        value = v;
    };
    return Read(key, to_string);
}

}
