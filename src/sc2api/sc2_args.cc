#include "sc2api/sc2_args.h"

#include "sc2utils/sc2_arg_parser.h"
#include "sc2utils/sc2_manage_process.h"
#include "sc2utils/sc2_property_reader.h"
#include "sc2utils/sc2_scan_directory.h"

#include <cassert>
#include <iostream>
#include <algorithm>

namespace sc2 {

const char* StarCraft2UserDirectory = "StarCraft II";
const char* StarCraft2ExecuteInfo = "ExecuteInfo.txt";

bool ParseFromFile(ProcessSettings& process_settings, GameSettings& game_settings, const std::string& file_name) {
    sc2::PropertyReader reader;

    if (!reader.LoadFile(file_name)) {
        return false;
    }

    reader.ReadString("executable", process_settings.process_path);
    int real_time = 0;
    reader.ReadInt("realtime", real_time);
    if (real_time) {
        process_settings.realtime = true;
    }
    else {
        process_settings.realtime = false;
    }
    reader.ReadInt("port", process_settings.port_start);
    reader.ReadString("map", game_settings.map_name);
    reader.ReadInt("timeout", process_settings.timeout_ms);
    return true;
}

bool FindLatestExe(std::string& path) {
    if (path.length() < 4)
        return false;

    static const char VersionsFolder[] = "Versions\\";
    static std::size_t BaseFolderNameLen = 10; // "Base00000\"
    std::size_t versions_pos = path.find(VersionsFolder);
    if (versions_pos == std::string::npos) {
        return DoesFileExist(path);
    }

    // Get the versions path.
    std::string versions_path = path;
    versions_path.erase(versions_path.begin() + versions_pos + sizeof(VersionsFolder) - 1, versions_path.end());

    // Get the exe name.
    std::string exe_name = path;
    exe_name.erase(exe_name.begin(), exe_name.begin() + versions_pos + sizeof(VersionsFolder) + BaseFolderNameLen - 1);

    // Get a list of all subfolders.
    std::vector<std::string> subfolders;
    scan_directory(versions_path.c_str(), subfolders, true, true);
    if (subfolders.size() < 1) {
        return DoesFileExist(path);
    }

    // Sort the subfolders list.
    std::sort(subfolders.begin(), subfolders.end());

    for (int folder_index = static_cast<int>(subfolders.size()) - 1; folder_index >= 0; --folder_index) {
        std::string test_path = subfolders[folder_index] + "\\" + exe_name;
        if (DoesFileExist(test_path)) {
            path = test_path;
            return true;
        }
    }

    return DoesFileExist(path);
}

#if defined(_WIN32)
    const char kDirectoryDivider = '\\';
#else
    const char kDirectoryDivider = '/';
#endif
    
bool ParseSettings(int argc, char* argv[], ProcessSettings& process_settings, GameSettings& game_settings) {
    assert(argc);
    ArgParser arg_parser(argv[0]);

    // First attempt to parse from the SC2 user directory.
    bool parsed = false;
    {
        std::string execute_info_filepath = GetUserDirectory();
        if (execute_info_filepath.length() > 0) {
            execute_info_filepath += kDirectoryDivider;
            execute_info_filepath += StarCraft2UserDirectory;
            execute_info_filepath += kDirectoryDivider;
            execute_info_filepath += StarCraft2ExecuteInfo;
            parsed = ParseFromFile(process_settings, game_settings, execute_info_filepath);
            if (parsed) {
                parsed = FindLatestExe(process_settings.process_path);
            }
        }
    }

    arg_parser.AddOptions({
        { "-e", "--executable", "The path to StarCraft II.", true },
        { "-s", "--step_size", "How many steps to take per call.", false },
        { "-p", "--port", "The port to make StarCraft II listen on.", false },
        { "-r", "--realtime", "Whether to run StarCraft II in  real time or not.", false },
        { "-m", "--map", "Which map to run.", false },
        { "-t", "--timeout", "Timeout for how long the library will block for a response.", false }
    });

    if (argc == 1 && !parsed) {
        // If there is no file and no command line arguments print how to use command line arguments.
        std::cout << "Please run StarCraft II before running this API" << std::endl;
        std::cout << "Command Line ";
        arg_parser.PrintUsage();
        std::cout << "For more options: ";
        std::cout << argv[0] << " --help" << std::endl << std::endl;
    }

    arg_parser.Parse(argc, argv);

    arg_parser.Get("executable", process_settings.process_path);
    if (process_settings.process_path.length() < 2) {
        std::cerr << "Unable to find executable." << std::endl;
        return false;
    }

    std::string step_size;
    if (arg_parser.Get("step_size", step_size)) {
        process_settings.step_size = atoi(step_size.c_str());
    }

    std::string realtime;
    if (arg_parser.Get("realtime", realtime)) {
        process_settings.realtime = realtime == "true" ? true : false;
    }

    std::string timeout;
    if (arg_parser.Get("timeout", timeout)) {
        process_settings.timeout_ms = atoi(timeout.c_str());
    }

    arg_parser.Get("map", game_settings.map_name);

    return true;
}

}
