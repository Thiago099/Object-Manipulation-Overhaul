#include "Application/ObjectReferenceFilterConfiguration.h"


bool ObjectReferenceFilter::Match(RE::TESObjectREFR* item) {
    if (!item) {
        return false;
    }
    bool isValid = false;
    for (const auto& filter : filters) {
        auto filterResult = filter->Run(item);
        auto kind = filter->GetFilterKind();
        if (filterResult) {
            isValid = kind;
        }
    }
    return isValid;
}

ObjectReferenceFilter& ObjectReferenceFilter::Install(std::string path, std::string regex) {
    ObjectReferenceFilter group;
    std::vector<Item> items;
    for (auto& fileName : File::Lookup(path, regex)) {
        logger::info("Loading config file: {}", fileName);
        for (auto& line : File::ReadAllLines(fileName)) {
            ProcessFilterItem(line, items);
        }
    }
    std::sort(items.begin(), items.end(), [](Item& a, Item& b) { 
        return a.priority < b.priority;
    });
    for (auto& item : items) {
        AddLine(item);
    }
    return group;
}

void ObjectReferenceFilter::ProcessFilterItem(std::string& line, std::vector<Item>& items) {
    auto itemGroups = bodyRegex.Match(line);
    if (itemGroups.size() >= 3) {
        auto name = Misc::ToLowerCase(itemGroups[2]);
        auto add = itemGroups[1] == "+";
        auto parameters = parametersRegex.MatchAll(itemGroups[3]);
        if (parameters.size() >= 1) {
            auto priority = 0;
            bool hasPriority = false;
            try {
                priority = std::stoi(parameters[0][1]);
                hasPriority = true;
            } catch (const std::invalid_argument&) {
            }
            if (hasPriority) {
                std::vector<std::string> props;
                if (parameters.size() >= 2) {
                    parameters.erase(parameters.begin());
                    for (auto& item : parameters) {
                        props.push_back(item[1]);
                    }
                }
                items.push_back(Item(priority, add, name, props));
            } else {
                logger::info("Expecting priority to be a number, got: {}", parameters[0][1]);
            }
        } else {
            logger::error("missing priority: {}", name);
        }
    }
}

void ObjectReferenceFilter::AddLine(Item& item) {

    // This violates open close principle, but i have no clue of how to do it otherwise in c++
    if (item.type == "all") {
        if (item.props.size() == 0) {
            auto current = new AllFilterItem();
            current->value = item.value;
            filters.push_back(current);

            //logger::trace("All filter, add: {}", item.value);
        } else {
            logger::error("invalid parameter count {} for {}", item.props.size(), item.type);
        }
    } else if (item.type == "formtype") {
        if (item.props.size() == 1) {
            auto current = new FormTypeFilterItem();
            current->value = item.value;
            current->formType = Misc::StringToFormType(item.props[0]);
            filters.push_back(current);
            //logger::trace("form type filter, add: {}, formType: {} ({})", item.value, current->formType,item.props[0]);
        } else {
            logger::error("invalid parameter count {} for {}", item.props.size(), item.type);
        }
    } else if (item.type == "formid") {
        RE::FormID formId = 0;
        if (item.props.size() == 2) {
            uint32_t localId = 0;
            try {
                localId = std::stoi(item.props[1], nullptr, 16);
            } 
            catch (const std::invalid_argument&) {
                logger::info("Invalid form id, {}", item.props[1]);
            
            }
            if (localId != 0) {
                const auto dataHandler = RE::TESDataHandler::GetSingleton();
                formId = dataHandler->LookupFormID(localId, item.props[0]);
            }
        } else if (item.props.size() == 1) {
            formId = std::stoi(item.props[0], nullptr, 16);
        } else {
            logger::error("invalid parameter count {} for {}", item.props.size(), item.type);
        }
        if (formId != 0) {
            auto current = new FormIdFilterItem();
            current->value = item.value;
            current->formId = formId;
            //logger::trace("Form Id Filter, add: {}, formId{:x}", item.value, formId);
            filters.push_back(current);
        }
    }
}

