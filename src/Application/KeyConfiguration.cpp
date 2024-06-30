#include "Application/KeyConfiguration.h"

void KeyConfiguration::Install(std::string path) {

    auto passiveInputManager = ObjectManipulationManager::GetPassiveInputManager();
    auto activeInputManager = ObjectManipulationManager::GetActiveInputManager();

    for (auto& line : File::ReadAllLines(path)) {
        auto itemGroups = bodyRegex.Match(line);
        if (itemGroups.size() == 3) {
            auto name = itemGroups[1];
            auto parameters = parametersRegex.MatchAll(itemGroups[2]);
            if (parameters.size() == 2) {
                auto device = parameters[0][1];
                auto key = parameters[1][1];

                if (passiveInputManager->HasSink(name)) {
                    passiveInputManager->AddSource(name, device, key);
                } else if (activeInputManager->HasSink(name)) {
                    activeInputManager->AddSource(name, device, key);
                }
            }
        }
    }
}
