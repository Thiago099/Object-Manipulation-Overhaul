#pragma once
#include "Lib/Regex.h"
#include "Lib/File.h"
#include "Lib/Misc.h"
#include "Lib/ObjectReferenceFilter.h"
#include "Lib/Json.h"
#include "Application/ObjectManipulationManager.h"


class ObjectReferenceFilterConfiguration;


struct PickFilter {
    float priority = 0.f;
    FilterItem* applyTo;
};

struct PlaceFilter {
    float priority = 0.f;
    FilterItem* applyTo;
    FilterItem* onTarget;
};


class ObjectReferenceFilterInGetter {
public:
    static JSON::Nullable<FilterItem::Action> ReadAction(JSON::Object& obj);
    static JSON::Nullable<JSON::Object> ReadApplyTo(JSON::Object& obj);
    static JSON::Nullable<JSON::Object> ReadOnTarget(JSON::Object& obj);
    static FilterItem* ReadObjectData(JSON::Object& obj, JSON::Object& subObj);
    static PickFilter CreatePick(JSON::Object obj);
    static PlaceFilter CreatePlace(JSON::Object obj);
};

class ObjectReferenceFilterConfiguration {
    static inline Regex parametersRegex = Regex("\\s*([^,\\s]+)\\s*(?:,|$)");
    public:
    static ObjectReferenceFilter& InstallPick(std::string path, std::string regex);
    static ObjectReferenceFilter& InstallPlace(std::string path, std::string regex);
};
