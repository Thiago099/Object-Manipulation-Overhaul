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

    auto actionStr = obj.Get<std::string>("Action");

    if (!actionStr) {
        return result;
    }

    logger::trace("Action: {}", *actionStr);
    FilterItem::Action action;

    if (Misc::IsEqual(*actionStr, "add")) {
        action = FilterItem::Add;
    } else if (Misc::IsEqual(*actionStr, "remove")) {
        action = FilterItem::Remove;
    } else {
        action = FilterItem::Modify;
    }

    auto priority = obj.Get<float>("Priority");

    if (!priority) {
        return result;
    }
    logger::trace("Priority: {}", *priority);

    auto applyTo = obj.Get<JSON::Object>("ApplyTo");
    if (!applyTo) {
        return result;
    }

    auto type = applyTo->Get<std::string>("Type");

    if (!type) {
        return result;
    }
    logger::trace("Type: {}", *type);

    if (Misc::IsEqual(*type, "all")) {
        auto current = new AllFilterItem();
        current->action = action;
        result.filter = current;
        return result;
    } 
    auto value = applyTo->Get<JSON::Array>("Value");

    if (!value) {
        return result;
    }

    if (Misc::IsEqual(*type, "formType")) {
        auto current = new FormTypeFilterItem();
        current->action = action;
        current->formType = value->GetAll<std::string, RE::FormType>([](std::string item) { return Misc::StringToFormType(item); });
        result.filter = current;
        return result;
    } 

    if (Misc::IsEqual(*type, "formId")) {
        auto modName = applyTo->Get<std::string>("ModName");
        logger::trace("ModName: {}", *modName);

        std::vector<RE::FormID> formIds;

        if (modName) {
            auto mod = *modName;
            formIds = value->GetAll<std::string, RE::FormID>([mod](std::string item) {
                RE::FormID formId = 0;
                uint32_t localId = 0;
                try {
                    localId = std::stoi(item, nullptr, 16);
                } catch (const std::invalid_argument&) {
                    logger::info("Invalid form id, {}", item);
                    return formId;
                }
                if (localId != 0) {
                    const auto dataHandler = RE::TESDataHandler::GetSingleton();
                    formId = dataHandler->LookupFormID(localId, mod);
                }
            return formId;
            });
        } else {
            formIds = value->GetAll<std::string, RE::FormID>([](std::string item) {
                RE::FormID formId = 0;
                formId = std::stoi(item, nullptr, 16);
                return formId;
            });
        }

       
        auto current = new FormIdFilterItem();
        current->action = action;

        formIds.erase(std::remove_if(formIds.begin(), formIds.end(), [](RE::FormID item) { return item == 0; }));

        current->formId = formIds;
        result.filter = current;
        return result;
    }

    logger::error("invalid filter type {}", *type);

    return result;
}

FilterItem* ObjectReferenceFilterInGetter::GetFilter() { return filter; }

float ObjectReferenceFilterInGetter::GetPriority() { return priority; }