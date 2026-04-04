#include "pch.h"

#include "Core/Filesystem.h"
#include "Core/Debug.h"

#include <fstream>
#include <sstream>
#include <filesystem>

std::string read_file_to_string(const char* path)
{
    std::ifstream ifs(path);
    if (!ifs)
    {
        std::string errorMsg = "[read_text_file]: file couldn't be opened!\npath: ";
        errorMsg += path;
        FATAL_CRASH(errorMsg.c_str());
    }

    std::stringstream buffer;
    buffer << ifs.rdbuf();
    return buffer.str();
}

std::string get_directory(const std::string& path)
{
    return std::filesystem::path(path).parent_path().generic_string();
}