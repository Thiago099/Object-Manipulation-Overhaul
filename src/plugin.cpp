#include "Plugin.h"


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


SKSEPluginLoad(const SKSE::LoadInterface* skse) {
    SKSE::Init(skse);
    SKSE::GetMessagingInterface()->RegisterListener(OnMessage);
    SetupLog();
    logger::info("Plugin loaded");
    UI::Register();

    logger::info("init");
    auto regex = Regex("([-+])\\s*([^\\s]*)\\s*\\(\\s*([^\\s]+)\\s*\\)");
    for (auto& fileName : File::Lookup(".\\Data", ".*_OMF\\.txt$")) {
        logger::info("file: {}", fileName);
        for (auto& line : File::ReadAllLines(fileName)) {
            auto match = regex.Match(line);
            if (match.size() == 4) {
                logger::info("{};{};{};", match[1], match[2], match[3]);
            }
        }
    }

    return true;
}