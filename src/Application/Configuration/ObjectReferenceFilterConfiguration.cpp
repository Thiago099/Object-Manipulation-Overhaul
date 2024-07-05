#include "Application/Configuration/ObjectReferenceFilterConfiguration.h"





ObjectReferenceFilter& ObjectReferenceFilterConfiguration::Install(std::string path, std::string regex) {
    ObjectReferenceFilter group;
    std::vector<ObjectReferenceFilterInGetter> items;
    auto filter = ObjectManipulationManager::GetRaycastReferenceFilter();
    for (auto& fileName : File::Lookup(path, regex)) {
        logger::info("Loading config file: {}", fileName);
        auto fileItems = JSON::ArrayFromFile(fileName);
        fileItems.FetchObject([&items](JSON::Object item) {
            auto current = ObjectReferenceFilterInGetter(item);
            if (current.GetFilter()) {
                items.push_back(current);
            }
        });
    }
    std::sort(items.begin(), items.end(), [](ObjectReferenceFilterInGetter& a, ObjectReferenceFilterInGetter& b) { 
        return a.GetPriority() < b.GetPriority();
    });
    for (auto& item : items) {
        filter->AddLine(item.GetFilter());
    }
    return group;
}



void ObjectReferenceFilterInGetter::Loop() {
    obj.FetchString(
        "action", [this](std::string item) { this->SetAction(item); },
        [](std::string error) { logger::error("Error on field action: {}", error); });
}
void ObjectReferenceFilterInGetter::SetAction(std::string actionSource) {
    if (Misc::IsEqual(actionSource, "add")) {
        action = true;
    } else if (Misc::IsEqual(actionSource, "remove")) {
        action = false;
    }
    logger::trace("action: {}", action);
    obj.FetchFloat(
        "priority", [this](float item) { this->SetPriority(item); }, [](std::string error) { logger::error("Error on field Priority: {}",error); });
}
void ObjectReferenceFilterInGetter::SetPriority(float prioritySource) {
    priority = prioritySource;
    logger::trace("priority: {}", priority);
    obj.FetchObject(
        "applyTo", [this](JSON::Object item) { this->SetApplyTo(item); },
        [](std::string error) { logger::error("Error on field applyTo: {}", error); });
}
void ObjectReferenceFilterInGetter::SetApplyTo(JSON::Object applyToSource) {
    applyTo = applyToSource;
    logger::trace("applyTo");
    applyTo.FetchString(
        "type", [this](std::string item) { this->SetType(item); },
        [](std::string error) { logger::error("Error on field type: {}", error); });
}
void ObjectReferenceFilterInGetter::SetType(std::string typeSource) {
    type = typeSource;
    logger::trace("type: {}", type);
    if (Misc::IsEqual(type, "all")) {
        auto current = new AllFilterItem();
        current->value = action;
        filter = current;
    } else {
        applyTo.FetchString(
            "value", [this](std::string item) { this->SetValue(item); },
            [](std::string error) { logger::error("Error on field value: {}", error); });
    }
}
void ObjectReferenceFilterInGetter::SetValue(std::string valueSource) {
    value = valueSource;
    logger::trace("value: {}", value);
    if (Misc::IsEqual(type, "formType")) {
        auto current = new FormTypeFilterItem();
        current->value = action;
        current->formType = Misc::StringToFormType(value);
        filter = current;
    } else if (Misc::IsEqual(type, "formId")) {
        RE::FormID formId = 0;
        applyTo.FetchString("modName", [this](std::string modNameSource) { modName = modNameSource; });
        if (!modName.empty()) {
            logger::trace("modName: {}", modName);
            uint32_t localId = 0;
            try {
                localId = std::stoi(value, nullptr, 16);
            } catch (const std::invalid_argument&) {
                logger::info("Invalid form id, {}", value);
                return;
            }
            if (localId != 0) {
                const auto dataHandler = RE::TESDataHandler::GetSingleton();
                formId = dataHandler->LookupFormID(localId, modName);
            }
        } else {
            formId = std::stoi(value, nullptr, 16);
        }
        auto current = new FormIdFilterItem();
        current->value = action;
        current->formId = formId;
        filter = current;
    } else {
        logger::error("Missing apply to value");
        return;
    }
}

FilterItem* ObjectReferenceFilterInGetter::GetFilter() { return filter; }

float ObjectReferenceFilterInGetter::GetPriority() { return priority; }