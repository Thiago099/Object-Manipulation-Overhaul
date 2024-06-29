#include "Plugin.h"

#include <filesystem>
#include <iostream>
#include <regex>
namespace fs = std::filesystem;

void OnMessage(SKSE::MessagingInterface::Message* message) {
    if (message->type == SKSE::MessagingInterface::kDataLoaded) {
        ObjectManipulationManager::Install();
    }
    if (message->type == SKSE::MessagingInterface::kPostLoadGame ||
        message->type == SKSE::MessagingInterface::kNewGame) {
        ObjectManipulationManager::Clean();
    }
}

std::vector<std::string> LookupFilesOnDirectory(std::string path, std::string regex) {
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

SKSEPluginLoad(const SKSE::LoadInterface* skse) {
    SKSE::Init(skse);
    SKSE::GetMessagingInterface()->RegisterListener(OnMessage);
    SetupLog();
    logger::info("Plugin loaded");
    UI::Register();

    logger::info("init");
    for (auto& fileName : LookupFilesOnDirectory(".\\Data", ".*_OMF\\.txt$")) {
        logger::info(":( {}", fileName);
    }

    return true;
}