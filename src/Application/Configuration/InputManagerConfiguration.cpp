#include "Application/Configuration/InputManagerConfiguration.h"

void InputManagerConfiguration::Install(std::string path) {

    auto passiveInputManager = ObjectManipulationManager::GetPassiveInputManager();
    auto activeInputManager = ObjectManipulationManager::GetActiveInputManager();

    for (auto& line : File::ReadAllLines(path)) {
        auto parameters = parametersRegex.MatchAll(line);
        if (parameters.size() == 3) {
            auto name = parameters[0][1];
            auto device = parameters[1][1];
            auto key = parameters[2][1];

            if (passiveInputManager->HasSink(name)) {
                passiveInputManager->AddSource(name, device, key);
            } else if (activeInputManager->HasSink(name)) {
                activeInputManager->AddSource(name, device, key);
            }
        }
    }
    if (activeInputManager->HasSource("ToggleMoveRotate")) {
        ObjectManipulationManager::SetdoToggleWithToggleKey(true);
    }
}
