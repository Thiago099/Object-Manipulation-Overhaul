#include "Application/Configuration/ObjectReferenceFilterConfiguration.h"





ObjectReferenceFilter& ObjectReferenceFilterConfiguration::Install(std::string path, std::string regex) {
    ObjectReferenceFilter group;
    std::vector<ObjectReferenceFilterInGetter> items;
    auto filter = ObjectManipulationManager::GetRaycastReferenceFilter();
    for (auto& fileName : File::Lookup(path, regex)) {
        logger::info("Loading config file: {}", fileName);
        auto fileItems = JSON::ArrayFromFile(fileName);

        for (auto item : fileItems.GetAll<JSON::Object>()) {
            auto current = ObjectReferenceFilterInGetter::Create(*item);
            if (current.GetFilter()) {
                items.push_back(current);
            }
        }


    }
    std::sort(items.begin(), items.end(), [](ObjectReferenceFilterInGetter& a, ObjectReferenceFilterInGetter& b) { 
        return a.GetPriority() < b.GetPriority();
    });
    for (auto& item : items) {
        filter->AddLine(item.GetFilter());
    }
    return group;
}



ObjectReferenceFilterInGetter ObjectReferenceFilterInGetter::Create(JSON::Object obj) {
    auto result = ObjectReferenceFilterInGetter();

    auto actionStr = obj.Get<std::string>("action");

    if (!actionStr) {
        logger::error("{}", actionStr.GetError("Action"));
        return result;
    }

    logger::trace("Action: {}", *actionStr);
    bool action;

    if (Misc::IsEqual(*actionStr, "add")) {
        action = true;
    } else if (Misc::IsEqual(*actionStr, "remove")) {
        action = false;
    }

    auto priority = obj.Get<float>("priority");

    if (!priority) {
        logger::error("{}", priority.GetError("Priority"));
        return result;
    }
    logger::trace("Priority: {}", *priority);

    auto applyTo = obj.Get<JSON::Object>("ApplyTo");
    if (!applyTo) {
        logger::error("{}", applyTo.GetError("ApplyTo"));
        return result;
    }

    auto type = applyTo->Get<std::string>("Type");

    if (!type) {
        logger::error("{}", type.GetError("Type"));
        return result;
    }
    logger::trace("Type: {}", *type);

    if (Misc::IsEqual(*type, "all")) {
        auto current = new AllFilterItem();
        current->value = action;
        result.filter = current;
        return result;
    } 
    auto value = applyTo->Get<std::string>("value");

    if (!value) {
        logger::error("{}", value.GetError("Value"));
        return result;
    }

    logger::trace("Value: {}", *value);

    if (Misc::IsEqual(*type, "formType")) {
        auto current = new FormTypeFilterItem();
        current->value = action;
        current->formType = Misc::StringToFormType(*value);
        result.filter = current;
        return result;
    } 

    if (Misc::IsEqual(*type, "formId")) {
        RE::FormID formId = 0;
        auto modName = applyTo->Get<std::string>("modName");
        if (modName) {
            logger::trace("ModName: {}", *modName);

            uint32_t localId = 0;
            try {
                localId = std::stoi(*value, nullptr, 16);
            } catch (const std::invalid_argument&) {
                logger::info("Invalid form id, {}", value);
                return result;
            }
            if (localId != 0) {
                const auto dataHandler = RE::TESDataHandler::GetSingleton();
                formId = dataHandler->LookupFormID(localId, *modName);
            }
        } else {
            formId = std::stoi(*value, nullptr, 16);
        }
        auto current = new FormIdFilterItem();
        current->value = action;
        current->formId = formId;
        result.filter = current;
        return result;
    }

    logger::error("invalid filter type {}", *type);

    return result;
}

FilterItem* ObjectReferenceFilterInGetter::GetFilter() { return filter; }

float ObjectReferenceFilterInGetter::GetPriority() { return priority; }