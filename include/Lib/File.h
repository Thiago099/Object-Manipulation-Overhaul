#pragma once
#include <filesystem>
#include <iostream>
#include <regex>
namespace File {
    std::vector<std::string> Lookup(std::string path, std::string regex);
    std::vector<std::string> ReadAllLines(std::string path);
}