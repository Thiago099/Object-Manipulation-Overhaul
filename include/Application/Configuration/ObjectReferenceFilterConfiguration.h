#pragma once
#include "Lib/Regex.h"
#include "Lib/File.h"
#include "Lib/Misc.h"
#include "Lib/ObjectReferenceFilter.h"
#include "Lib/Json.h"
#include "Application/ObjectManipulationManager.h"



class ObjectReferenceFilterInGetter {
private:
    JSON::Object obj;
    bool action = false;
    float priority = 0.f;
    JSON::Object applyTo;
    std::string type = "";
    std::string value = "";
    std::string modName = "";
    FilterItem* filter;
    void Loop();
    void SetAction(std::string action);
    void SetPriority(float applyTo);
    void SetApplyTo(JSON::Object applyToSource);
    void SetType(std::string applyToSource);
    void SetValue(std::string valueSource);

public:
    ObjectReferenceFilterInGetter(JSON::Object obj): obj(obj) {
        Loop();
    }
    FilterItem* GetFilter();
    float GetPriority();
};

class ObjectReferenceFilterConfiguration {
    static inline Regex parametersRegex = Regex("\\s*([^,\\s]+)\\s*(?:,|$)");
    public:
    static ObjectReferenceFilter& Install(std::string path, std::string regex);
};
