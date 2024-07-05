#include "Application/Configuration/ObjectReferenceFilterConfiguration.h"





ObjectReferenceFilter& ObjectReferenceFilterConfiguration::Install(std::string path, std::string regex) {
    ObjectReferenceFilter group;
    std::vector<ObjectReferenceFilterInGetter> items;
    auto filter = ObjectManipulationManager::GetRaycastReferenceFilter();
    for (auto& fileName : File::Lookup(path, regex)) {
        logger::info("Loading config file: {}", fileName);
        auto fileItems = JSON::ArrayFromFile(fileName);

        for (auto item : fileItems.Fetch<JSON::Object>()) {
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
    auto actionStr = obj.Fetch<std::string>("action");
    auto priority = obj.Fetch<float>("Priority");
    auto type = obj.Fetch<std::string>("Type");
    bool action;
    if (actionStr) {
        logger::info("{}", *actionStr);
    }
    if (actionStr && priority && type) {
        logger::info("{}", *priority);

        if (Misc::IsEqual(*actionStr, "add")) {
            action = true;
        } else if (Misc::IsEqual(*actionStr, "remove")) {
            action = false;
        }
        if (Misc::IsEqual(*type, "all")) {
            auto current = new AllFilterItem();
            current->value = action;
            result.filter = current;
        } else {
            auto applyTo = obj.Fetch<JSON::Object>("ApplyTo");
            if (applyTo) {
                auto value = applyTo->Fetch<std::string>("value");
                if (value) {
                    if (Misc::IsEqual(*type, "formType")) {
                        auto current = new FormTypeFilterItem();
                        current->value = action;
                        current->formType = Misc::StringToFormType(*value);
                        result.filter = current;
                    } else if (Misc::IsEqual(*type, "formId")) {
                        RE::FormID formId = 0;
                        auto modName = applyTo->Fetch<std::string>("modName");
                        if (modName) {
                            logger::trace("modName: {}", modName);
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
                    } else {
                        logger::error("Missing apply to value");
                        return result;
                    }
                }
            }
        }
    }
    return result;
}

FilterItem* ObjectReferenceFilterInGetter::GetFilter() { return filter; }

float ObjectReferenceFilterInGetter::GetPriority() { return priority; }