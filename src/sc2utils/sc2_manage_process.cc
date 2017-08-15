#include "sc2utils/sc2_manage_process.h"

#include <iostream>
#include <cassert>
#include <stdio.h>
#include <chrono>
#include <thread>
#include <fstream>

#if defined(_WIN32)

// Windows headers for process manipulation.
#include <windows.h>
#include <shlobj.h>
#include <conio.h>
#include <tchar.h>
#include <vector>
#include <string>
#include <codecvt>
#include <locale>

#elif defined(__APPLE__)

// Mac headers for process manipulation.
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <errno.h>
#include <cstring>
#include <unistd.h>
#include <pwd.h>
#include <termios.h>
#include <mach-o/dyld.h>
#include <ctype.h>

#include <Carbon/Carbon.h>

#elif defined(__linux__)

// Linux headers for process manipulation.
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <errno.h>
#include <cstring>
#include <unistd.h>
#include <pwd.h>
#include <termios.h>
#include <stropts.h>
#include <linux/limits.h>

#else
#error "Unsupported platform"
#endif

namespace sc2 {

void SleepFor(unsigned int ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

bool DoesFileExist(const std::string& path) {
    return std::ifstream(path).good();
}

bool HasExtension (const std::string& map_name, const std::string& extention) {
    if (map_name.size() < extention.size())
        return false;

    return std::equal(
        map_name.end() - extention.size(),
        map_name.end(),
        extention.begin(),
        extention.end(),
        [](char a, char b) {
            return tolower(a) == tolower(b);
        }
    );
}

#ifdef _WIN32

struct WindowsProcess {
    STARTUPINFO si_;
    PROCESS_INFORMATION pi_;
};
std::vector<WindowsProcess> windows_processes;

static int GetIndexOfProcess(uint64_t processe_id) {
    for (int i = 0; i < windows_processes.size(); ++i) {
        if ((DWORD)processe_id == windows_processes[i].pi_.dwProcessId)
            return i;
    }
    return -1;
}

std::string GetUserDirectory() {
    unsigned int csidl = CSIDL_PERSONAL;
    WCHAR windowsPath[MAX_PATH];

    HRESULT result = SHGetFolderPathW(
        nullptr,
        csidl,
        nullptr,
        SHGFP_TYPE_CURRENT,
        windowsPath
    );

    if (result == S_OK) {
        std::wstring_convert<std::codecvt_utf8_utf16<WCHAR>, WCHAR> convertor;
        return convertor.to_bytes(windowsPath);
    }

    return std::string();
}

static std::string GetExePath() {
    WCHAR windowsPath[MAX_PATH];

    DWORD length = GetModuleFileNameW(0, windowsPath, MAX_PATH);
    if (length > 0) {
        std::wstring_convert<std::codecvt_utf8_utf16<WCHAR>, WCHAR> convertor;
        return convertor.to_bytes(windowsPath);
    }

    return std::string();
}

std::string GetLibraryMapsDirectory() {
    std::string result = GetExePath();
    result = result.substr(0, result.find_last_of("\\"));
    result = result.substr(0, result.find_last_of("\\"));
    result = result.substr(0, result.find_last_of("\\"));
    result += "\\maps\\";
    return result;
}

std::string GetGameMapsDirectory(const std::string& process_path) {
    std::string result = process_path;
    result = result.substr(0, result.find_last_of("\\"));
    result = result.substr(0, result.find_last_of("\\"));
    result = result.substr(0, result.find_last_of("\\"));
    result += "\\maps\\";
    return result;
}

BOOL WINAPI ConsoleHandlerRoutine(DWORD /*dwCtrlType*/) {
    while (windows_processes.size()) {
        uint64_t pid = static_cast<uint64_t>(windows_processes[windows_processes.size() - 1].pi_.dwProcessId);
        if (!TerminateProcess(pid))
            windows_processes.pop_back();
    }
    return FALSE;
}

uint64_t StartProcess(const std::string& process_path, const std::vector<std::string>& command_line) {
    static const unsigned int buffer_size = (1 << 16) + 1;

    WindowsProcess process;
    std::memset(&process, 0, sizeof(process));
    process.si_.cb = sizeof(process.si_);

    char current_directory[buffer_size];
    char support_directory[buffer_size];
    GetCurrentDirectory(buffer_size, current_directory);
    strcpy_s(support_directory, process_path.c_str());
    int slashcount = 0;
    for (size_t i = strlen(support_directory); i > 0 && slashcount < 3; --i) {
        if (support_directory[i] == '/' || support_directory[i] == '\\') {
            ++slashcount;
            if (slashcount > 1)
                support_directory[i + 1] = 0;
        }
    }
    if (slashcount != 3)
        return false;

    std::string exe_name = process_path.substr(process_path.find_last_of("/\\"));
    bool is64bit = exe_name.find("_x64") != std::string::npos;
    if (is64bit)
        strcat_s(support_directory, "Support64");
    else
        strcat_s(support_directory, "Support");

    SetCurrentDirectory(support_directory);

    char buffer[buffer_size];
    std::memset(buffer, 0, buffer_size);
    for (int i = 0; i < command_line.size(); ++i) {
        strcat_s(buffer, " ");
        strcat_s(buffer, command_line[i].c_str());
    }

    if (!CreateProcess(process_path.c_str(), // Module name
        buffer,                 // Command line
        NULL,                   // Process handle not inheritable
        NULL,                   // Thread handle not inheritable
        FALSE,                  // Set handle inheritance to FALSE
        0,                      // No creation flags
        NULL,                   // Use parent's environment block
        NULL,                   // Use parent's starting directory
        &process.si_,           // Pointer to STARTUPINFO structure
        &process.pi_)          // Pointer to PROCESS_INFORMATION structure
        )
    {
        SetCurrentDirectory(current_directory);
        return uint64_t(0);
    }

    windows_processes.push_back(process);
    SetCurrentDirectory(current_directory);
    SleepFor(1000);

    // Hook.
    SetConsoleCtrlHandler(ConsoleHandlerRoutine, TRUE);

    return static_cast<uint64_t>(process.pi_.dwProcessId);
}

bool IsProcessRunning(uint64_t process_id) {
    int index = GetIndexOfProcess(process_id);
    if (index < 0)
        return false;

    DWORD exit_code;
    if (!GetExitCodeProcess(windows_processes[index].pi_.hProcess, &exit_code))
        return false;

    return exit_code == STILL_ACTIVE;
}

bool TerminateProcess(uint64_t process_id) {
    int index = GetIndexOfProcess(process_id);
    if (index < 0)
        return false;

    ::TerminateProcess(windows_processes[index].pi_.hProcess, -1);
    WaitForSingleObject(windows_processes[index].pi_.hProcess, 120 * 1000);

    process_id = 0LL;

    CloseHandle(windows_processes[index].pi_.hProcess);
    CloseHandle(windows_processes[index].pi_.hThread);

    windows_processes.erase(windows_processes.begin() + index);

    return true;
}

bool IsInDebugger() {
    return IsDebuggerPresent() == TRUE;
}

#elif defined(__linux__) || defined(__APPLE__)

std::vector<uint64_t>& GetPids() {
    static std::vector<uint64_t> pids;
    return pids;
}

void AddPid(uint64_t pid) {
    GetPids().push_back(pid);
}

void RemovePid(uint64_t pid) {
    std::vector<uint64_t>& pids = GetPids();
    for (size_t i = 0; i < pids.size(); ++i) {
        if (pids[i] == pid) {
            pids.erase(pids.begin() + i);
            return;
        }
    }
}

void KillRunningProcesses(int signum) {
    // Get copy since pids will be iterated and removed.
    std::vector<uint64_t> pids = GetPids();
    for (auto pid : pids) {
        TerminateProcess(pid);
    }
    exit(-1);
}

#if defined(__linux__)
std::string GetUserDirectory() {
    const char* home_directory = getenv("HOME");
    if (!home_directory)
        home_directory = getpwuid(getuid())->pw_dir;
    return std::string(home_directory);
}
#else

void GetDirectory(std::string& path, uint32_t folderType, short domain) {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"

    FSRef fsref;
    OSErr err = FSFindFolder(domain, folderType, false, &fsref);

    if (err == noErr) {
        char pathBuffer[PATH_MAX];
        FSRefMakePath(&fsref, reinterpret_cast<unsigned char*>(pathBuffer), PATH_MAX);
        path = pathBuffer;
    }

#pragma clang diagnostic pop
}

std::string GetUserDirectory() {
    std::string result;
    GetDirectory(result, kApplicationSupportFolderType, kUserDomain);
    result += "/Blizzard";
    return result;
}
#endif

static std::string GetExePath() {
#if defined(__linux__)
    char path[PATH_MAX + 1] = { 0 };
    if (readlink("/proc/self/exe", path, PATH_MAX) == -1)
        return std::string();

    return std::string(path);
#else
    char path[PATH_MAX];
    uint32_t size = sizeof(path);
    if (_NSGetExecutablePath(path, &size) != 0)
        return std::string();

    return std::string(path);
#endif
}

std::string GetLibraryMapsDirectory() {
    std::string result = GetExePath();

    char* resolvedPath = realpath(result.c_str(), nullptr);
    if (resolvedPath != nullptr) {
        result = resolvedPath;
        free(resolvedPath);
    }

    result = result.substr(0, result.find_last_of("/"));
    result = result.substr(0, result.find_last_of("/"));
    result = result.substr(0, result.find_last_of("/"));
    result += "/maps/";
    return result;
}

std::string GetGameMapsDirectory(const std::string& process_path) {
    std::string result = process_path;
#if defined(__linux__)
    result = result.substr(0, result.find_last_of("/"));
    result = result.substr(0, result.find_last_of("/"));
    result = result.substr(0, result.find_last_of("/"));
#else
    result = result.substr(0, result.find_last_of("/"));
    result = result.substr(0, result.find_last_of("/"));
    result = result.substr(0, result.find_last_of("/"));
    result = result.substr(0, result.find_last_of("/"));
    result = result.substr(0, result.find_last_of("/"));
    result = result.substr(0, result.find_last_of("/"));
#endif
    result += "/maps/";
    return result;
}

int _kbhit() {
    static const int STDIN = 0;
    static bool initialized = false;

    if (!initialized) {
        // Use termios to turn off line buffering
        termios term;
        tcgetattr(STDIN, &term);
        term.c_lflag &= ~ICANON;
        tcsetattr(STDIN, TCSANOW, &term);
        setbuf(stdin, NULL);
        initialized = true;
    }

    int bytesWaiting;
    ioctl(STDIN, FIONREAD, &bytesWaiting);
    return bytesWaiting;
}

uint64_t StartProcess(const std::string& process_path, const std::vector<std::string>& command_line) {
    std::vector<char*> char_list;
    // execve expects the process path to be the first argument in the list.
    char_list.push_back(const_cast<char*>(process_path.c_str()));
    for (auto& s : command_line) {
        char_list.push_back(const_cast<char*>(s.c_str()));
    }

    // List needs to be null terminated for execve.
    char_list.push_back(nullptr);

    // Start the process.
    pid_t p = fork();
    if (p == 0) {
        if (execve(char_list[0], &char_list[0], nullptr) == -1) {
            std::cerr << "Failed to execute process " << char_list[0]
                << " error: " << strerror(errno) << std::endl;
            exit(-1);
        }

        SleepFor(1000);
    }

    static bool hook_sig = true;
    if (hook_sig) {
        struct sigaction action;
        memset(&action, 0, sizeof(struct sigaction));
        // Kill process started by this process of SIGTERM and SIGSEGV.
        action.sa_handler = KillRunningProcesses;
        sigaction(SIGTERM, &action, NULL);
        sigaction(SIGSEGV, &action, NULL);
        sigaction(SIGINT, &action, NULL);
        hook_sig = false;
    }

    AddPid(p);

    return p;
}

bool IsProcessRunning(uint64_t process_id) {
#if defined(__linux__)
    struct stat sts;
    char* proc;
    asprintf(&proc, "/proc/%lu", process_id);
    if (stat(proc, &sts) == -1 && errno == ENOENT) {
        std::cerr << "Process not running" << std::endl;
        return false;
    }
    return true;
#else
    if (process_id == 0)
        return false;

    return kill(process_id, 0) != -1;
#endif
}

bool TerminateProcess(uint64_t process_id) {
    if (kill(process_id, SIGKILL) == -1) {
        return false;
    }
    RemovePid(process_id);
    return true;
}

bool IsInDebugger() {
    return false;
}

#endif

bool PollKeyPress() {
    if (_kbhit())
        return true;
    // TODO: Consume the character.
    return false;
}

}
