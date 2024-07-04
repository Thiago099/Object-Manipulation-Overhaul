#include "Application/Configuration/ObjectReferenceFilterConfiguration.h"




ObjectReferenceFilter& ObjectReferenceFilterConfiguration::Install(std::string path, std::string regex) {
    ObjectReferenceFilter group;
    std::vector<Item> items;
    auto filter = ObjectManipulationManager::GetRaycastReferenceFilter();
    for (auto& fileName : File::Lookup(path, regex)) {
        logger::info("Loading config file: {}", fileName);
        auto json = JSON::ArrayFromFile(fileName);
        if (json.FetchObject(0)) {
            auto obj = json.GetObject();
            if (obj.FetchString("action")) {
                auto action = obj.GetString();
                logger::info("Action: {}", action);
            }
        }

        //for (auto& line : File::ReadAllLines(fileName)) {
        //    ProcessFilterItem(line, items);
        //}
    }
    std::sort(items.begin(), items.end(), [](Item& a, Item& b) { 
        return a.priority < b.priority;
    });
    for (auto& item : items) {
        filter->AddLine(item);
    }
    return group;
}

void ObjectReferenceFilterConfiguration::ProcessFilterItem(std::string& line, std::vector<Item>& items) {
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
                auto filter = itemGroups[4];

                if (filter.size() > 0) {
                    auto filters = parametersRegex.MatchAll(itemGroups[4]);
                    if (filters.size() > 0) {
                        for (auto& filterMatch : filters) {
                            auto filter = filterMatch[1];
                            logger::trace("filter: {}", filter);
                        }
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
