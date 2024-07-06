#pragma once
#include "Lib/Regex.h"
#include "Lib/File.h"
#include "Lib/Misc.h"
#include "Lib/ObjectReferenceFilter.h"
#include "Lib/Json.h"
#include "Application/ObjectManipulationManager.h"


class ObjectReferenceFilterConfiguration;

class ObjectReferenceFilterInGetter {
private:
    float priority = 0.f;
    FilterItem* filter;
public:
    static ObjectReferenceFilterInGetter Create(JSON::Object obj);
    FilterItem* GetFilter();
    float GetPriority();
};

class ObjectReferenceFilterConfiguration {
    static inline Regex parametersRegex = Regex("\\s*([^,\\s]+)\\s*(?:,|$)");
    public:
    static ObjectReferenceFilter& Install(std::string path, std::string regex);
};
