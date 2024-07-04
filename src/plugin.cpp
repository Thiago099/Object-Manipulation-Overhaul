#include "Plugin.h"


namespace fs = std::filesystem;

void OnMessage(SKSE::MessagingInterface::Message* message) {
    if (message->type == SKSE::MessagingInterface::kDataLoaded) {
        ObjectReferenceFilterConfiguration::Install(".\\Data\\Object Manipulation Overhaul", ".*_OMO\\.json$");
        ObjectManipulationManager::Install();
        InputManagerConfiguration::Install(".\\Data\\Object Manipulation Overhaul\\KeyConfiguration.txt");

    }
    if (message->type == SKSE::MessagingInterface::kPreLoadGame) {
        ObjectManipulationManager::Clean();
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
    Papyrus::Install();

    logger::info("Plugin loaded");
    logger::info("init");

    return true;
}