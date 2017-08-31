#pragma once

#include <cstdint>
#include <vector>
#include <string>

namespace sc2 {

bool DoesFileExist(const std::string& path);
bool HasExtension(const std::string& path, const std::string& extention);
uint64_t StartProcess(const std::string& process_path, const std::vector<std::string>& command_line);
bool IsProcessRunning(uint64_t process_id);
bool TerminateProcess(uint64_t process_id);
bool IsInDebugger();
void SleepFor(unsigned int ms);
bool PollKeyPress();
std::string GetUserDirectory();
std::string GetLibraryMapsDirectory();
std::string GetGameMapsDirectory(const std::string& process_path);

bool FindLatestExe(std::string& path);
bool FindBaseExe(std::string& path, uint32_t base_build);

}
