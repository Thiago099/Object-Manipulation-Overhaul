#include "Lib/File.h"


std::vector<std::string> File::Lookup(std::string path, std::string regex) {
    std::regex regexPattern(regex);
    std::vector<std::string> fileList;
    for (const auto& dirEntry : std::filesystem::directory_iterator(path)) {
        auto str = dirEntry.path().filename().string();
        if (std::regex_match(str, regexPattern)) {
            fileList.push_back(dirEntry.path().string());
        }
    }
    return fileList;
}


std::vector<std::string> File::ReadAllLines(std::string path) {
    std::ifstream file(path);
    std::vector<std::string> result;
    if (file.is_open()) {
        std::string line;
        while (getline(file, line)) {
            result.push_back(line);
        }
        file.close();
    }
    return result;
}