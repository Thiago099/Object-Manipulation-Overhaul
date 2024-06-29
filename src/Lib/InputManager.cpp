#include "Lib/InputManager.h"


bool InputManager::ProcessInput(RE::ButtonEvent* button) {
    auto device = button->GetDevice();
    auto idcode = button->GetIDCode();
    auto deviceIterator = inputs.find(device);
    if (deviceIterator == inputs.end()) {
        return false;
    }
    auto deviceData = deviceIterator->second;
    auto inputIterator = deviceData.find(idcode);
    if (inputIterator == deviceData.end()) {
        return false;
    }
    auto actionNames = inputIterator->second;
    bool actionsExecuted = false;
    for (const auto& actionName : actionNames) {
        auto actionIterator = actions.find(actionName);
        if (actionIterator == actions.end()) {
            continue;
        }
        const auto& action = actionIterator->second;
        action(button);
        actionsExecuted = true;
    }
    return actionsExecuted;
}
uint32_t InputManager::GetId(std::string key) {
    auto idIterator = idMap.find(key);
    if (idIterator == idMap.end()) {
        auto result = auto_increment;
        idMap[key] = auto_increment++;
        return result;
    }
    return idIterator->second;
}

void InputManager::AddAction(std::string actionName, std::function<void(RE::ButtonEvent*)> const & callback) {
    actions.insert(std::make_pair(GetId(actionName),callback));
}

void InputManager::AddBinding(std::string actionName, std::string deviceName, std::string buttonName) {
    auto deviceIterator = deviceMap.find(deviceName);
    if (deviceIterator == deviceMap.end()) {
        return;
    }
    uint32_t device = deviceIterator->second;
    uint32_t idcode;
    switch (device) {
        case RE::INPUT_DEVICE::kKeyboard: {
            auto keyIterator = keyboardMap.find(deviceName);
            if (keyIterator == keyboardMap.end()) {
                return;
            }
            idcode = keyIterator->second;
        } break;
        case RE::INPUT_DEVICE::kMouse: {
            auto keyIterator = mouseMap.find(deviceName);
            if (keyIterator == mouseMap.end()) {
                return;
            }
            idcode = keyIterator->second;
        }
        case RE::INPUT_DEVICE::kGamepad: {
            auto keyIterator = mouseMap.find(deviceName);
            if (keyIterator == mouseMap.end()) {
                return;
            }
            idcode = keyIterator->second;
        }
        default:
            break;
    }
    inputs[device][idcode].push_back(GetId(actionName));
}
