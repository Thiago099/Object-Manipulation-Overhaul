#include "Plugin.h"


namespace fs = std::filesystem;

void OnMessage(SKSE::MessagingInterface::Message* message) {
    if (message->type == SKSE::MessagingInterface::kDataLoaded) {
        ObjectReferenceFilterConfiguration::InstallPick(".\\Data\\Object Manipulation Overhaul",".*_PICK\\.json$");
        ObjectReferenceFilterConfiguration::InstallPlace(".\\Data\\Object Manipulation Overhaul", ".*_PLACE\\.json$");
        ObjectManipulationManager::Install();
        InputManagerConfiguration::Install(".\\Data\\Object Manipulation Overhaul\\KeyConfiguration.txt");

    }
    if (message->type == SKSE::MessagingInterface::kPreLoadGame ||
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

    auto serialization = SKSE::GetSerializationInterface();
    serialization->SetUniqueID('OMO9');
    serialization->SetSaveCallback(Persistence::SaveCallback);
    serialization->SetLoadCallback(Persistence::LoadCallback);

    return true;
}