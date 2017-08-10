#include "sc2utils/sc2_scan_directory.h"

#include <iostream>
#include <cassert>
#include <cstring>
#include <string>

#ifdef _WIN32
#include "dirent.h"
#else
#include <dirent.h>
#endif

namespace sc2 {

int scan_directory(const char* directory_path, std::vector<std::string>& files, bool full_path, bool list_directories) {
    if (!directory_path || !*directory_path)
        return 0;

    DIR *dir;
    dir = opendir(directory_path);
    if (!dir)
        return 0;

    struct dirent *ent;

    while ((ent = readdir (dir)) != NULL) {
        switch (ent->d_type) {
        case DT_REG: {
            if (list_directories) {
                continue;
            }

            if (!full_path) {
                files.push_back(ent->d_name);
            }
            else {
                files.push_back(std::string(directory_path) + std::string(ent->d_name));
            }
        }
        case DT_DIR: {
            if (!list_directories || !*ent->d_name) {
                continue;
            }

            if (std::strcmp(ent->d_name, ".") == 0 || std::strcmp(ent->d_name, "..") == 0) {
                continue;
            }

            if (!full_path) {
                files.push_back(ent->d_name);
            }
            else {
                files.push_back(std::string(directory_path) + std::string(ent->d_name));
            }
        }
        break;
        }
    }

    return (int)files.size();
}

}
