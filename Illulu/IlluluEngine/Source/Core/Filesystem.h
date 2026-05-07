#pragma once

//-----------------------------------------------------------------------------------------
/**
 * \file       Filesystem.h
 * \author     Ilja Buran
 * \date       2025/05/07
 * \brief      Declares helper functions for reading files and extracting directory paths.
 */
 //-----------------------------------------------------------------------------------------

#include <string>

std::string read_file_to_string(const char* path);
std::string get_directory(const std::string& path);