#include "Plugin.h"


void OnMessage(SKSE::MessagingInterface::Message* message) {
    if (message->type == SKSE::MessagingInterface::kDataLoaded) {
        ObjectManipulationManager::Install();
    }
    if (message->type == SKSE::MessagingInterface::kPostLoadGame || message->type == SKSE::MessagingInterface::kNewGame) {
    }
}


SKSEPluginLoad(const SKSE::LoadInterface *skse) {
    SKSE::Init(skse);
    SKSE::GetMessagingInterface()->RegisterListener(OnMessage);
    SetupLog();
    logger::info("Plugin loaded");
    UI::Register();

    logger::info("init");   


    return true;
}