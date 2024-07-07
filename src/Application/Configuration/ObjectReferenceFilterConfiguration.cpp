#include "Application/Configuration/ObjectReferenceFilterConfiguration.h"





ObjectReferenceFilter& ObjectReferenceFilterConfiguration::InstallPick(std::string path, std::string regex) {
    ObjectReferenceFilter group;
    std::vector<PickFilter> items;
    auto filter = ObjectManipulationManager::GetPickFilter();
    for (auto& fileName : File::Lookup(path, regex)) {
        logger::info("Loading pick config file: {}", fileName);
        auto fileItems = JSON::ArrayFromFile(fileName);
        for (auto item : fileItems.GetAll<JSON::Object>()) {
            auto current = ObjectReferenceFilterInGetter::CreatePick(*item);
            if (current.applyTo) {
                items.push_back(current);
            }
        }
    }
    std::sort(items.begin(), items.end(), [](PickFilter& a, PickFilter& b) { 
        return a.priority < b.priority;
    });
    for (auto& item : items) {
        filter->AddLine(item.applyTo);
    }
    return group;
}
ObjectReferenceFilter& ObjectReferenceFilterConfiguration::InstallPlace(std::string path, std::string regex) {
    ObjectReferenceFilter group;
    std::vector<PlaceFilter> items;
    auto filter = ObjectManipulationManager::GetPlaceFilter();
    for (auto& fileName : File::Lookup(path, regex)) {
        logger::info("Loading pick config file: {}", fileName);
        auto fileItems = JSON::ArrayFromFile(fileName);
        for (auto item : fileItems.GetAll<JSON::Object>()) {
            auto current = ObjectReferenceFilterInGetter::CreatePlace(*item);
            if (current.applyTo) {
                items.push_back(current);
            }
        }
    }
    std::sort(items.begin(), items.end(), [](PlaceFilter& a, PlaceFilter& b) { return a.priority < b.priority; });
    for (auto& item : items) {
        filter->AddLine(item.applyTo, item.onTarget);
    }
    return group;
}



JSON::Nullable<FilterItem::Action> ObjectReferenceFilterInGetter::ReadAction(JSON::Object& obj) {

    auto action = obj.Get<std::string>("Action");
    auto result = JSON::Nullable<FilterItem::Action>();

    if (!action) {
        return result;
    }

    if (Misc::IsEqual(*action, "add")) {
        result.Set(FilterItem::Add);
    } else if (Misc::IsEqual(*action, "remove")) {
        result.Set(FilterItem::Remove);
    }

    return result;
}



JSON::Nullable<JSON::Object> ObjectReferenceFilterInGetter::ReadApplyTo(JSON::Object& obj) {
    return obj.Get<JSON::Object>("ApplyTo");
}

JSON::Nullable<JSON::Object> ObjectReferenceFilterInGetter::ReadOnTarget(JSON::Object& obj) {
    return obj.Get<JSON::Object>("ReadOn");
}


FilterItem* ObjectReferenceFilterInGetter::ReadObjectData(JSON::Object& obj, JSON::Object& subObj) {
    auto action = ReadAction(obj);
    auto type = subObj.Get<std::string>("Type");

    if (!type || !action) {
        logger::error("configuration incomplete for item");
        return nullptr;
    }

    if (Misc::IsEqual(*type, "all")) {
        auto current = new AllFilterItem();
        current->action = *action;
        return current;
    }
    auto value = subObj.Get<JSON::Array>("Value");

    if (!value) {
        return nullptr;
    }

    if (Misc::IsEqual(*type, "formType")) {
        auto current = new FormTypeFilterItem();
        current->action = *action;
        current->formType =
            value->GetAll<std::string, RE::FormType>([](std::string item) { return Misc::StringToFormType(item); });
        return current;
    }

    if (Misc::IsEqual(*type, "formId")) {
        auto modName = subObj.Get<std::string>("ModName");
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
        current->action = *action;

        formIds.erase(std::remove_if(formIds.begin(), formIds.end(), [](RE::FormID item) { return item == 0; }));

        current->formId = formIds;
        return current;
    }
    return nullptr;
}

PickFilter ObjectReferenceFilterInGetter::CreatePick(JSON::Object obj) {
    auto priority = obj.Get<float>("Priority");
    auto applyTo = obj.Get<JSON::Object>("ApplyTo");
    if (!applyTo || !priority) {
        logger::error("Invalid pick configuration");
        return PickFilter();
    }
    logger::trace("loaded pick configuration");
    auto result = PickFilter();
    result.priority = priority;
    result.applyTo = ReadObjectData(obj, *applyTo);
    return result;
}
PlaceFilter ObjectReferenceFilterInGetter::CreatePlace(JSON::Object obj) {
    auto priority = obj.Get<float>("Priority");
    auto applyTo = obj.Get<JSON::Object>("ApplyTo");
    auto onTarget = obj.Get<JSON::Object>("OnTarget");
    if (!applyTo || !onTarget || !priority) {
        logger::error("Invalid place configuration");
        return PlaceFilter();
    }
    logger::trace("loaded place configuration");
    auto result = PlaceFilter();
    result.priority = priority;
    result.applyTo = ReadObjectData(obj, *applyTo);
    result.onTarget = ReadObjectData(obj, *onTarget);
    return result;
}
