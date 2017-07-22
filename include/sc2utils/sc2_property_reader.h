#pragma once

#include <string>
#include <unordered_map>
#include <functional>

namespace sc2 {

class PropertyReader
{
public:
    typedef std::unordered_map<std::string, std::string>::const_iterator ConstMapIterator;
    typedef std::unordered_map<std::string, std::string> PropertiesMap;

    PropertyReader();
    PropertyReader(const std::string& file_name);
    ~PropertyReader();

    bool IsLoaded() const;
    bool LoadFile(const std::string& file_name);
    void Free();

    // Return false if no properties loaded or value does not exist in file
    bool Read(const std::string& key, std::function<void(const std::string& v)> convert);
    bool ReadInt(const std::string& key, int& value);
    bool ReadFloat(const std::string& key, float& value);
    bool ReadString(const std::string& key, std::string& value);

private:
    PropertiesMap properties_;
    bool file_read_;
};

}
