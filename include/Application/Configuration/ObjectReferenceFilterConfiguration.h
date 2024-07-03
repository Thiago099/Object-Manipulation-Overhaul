#pragma once
#include "Lib/Regex.h"
#include "Lib/File.h"
#include "Lib/Misc.h"
#include "Lib/ObjectReferenceFilter.h"
#include "Application/ObjectManipulationManager.h"


class ObjectReferenceFilterConfiguration {
    static inline Regex bodyRegex = Regex("^\\s*([-+])\\s*([^\\s]*)\\s*\\(\\s*([^)]*)\\s*\\)");
    static inline Regex parametersRegex = Regex("\\s*([^,\\s]+)\\s*(?:,|$)");
    static void ProcessFilterItem(std::string& line, std::vector<Item>& items);
    public:
        static ObjectReferenceFilter& Install(std::string path, std::string regex);
};
