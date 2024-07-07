
#pragma once
#include "Lib/Misc.h"
#include "Lib/Raycast.h"


class FilterItem {
public:
    enum Action {
        Add,
        Remove
    };

    Action action;
    Action GetFilterKind() const { return action; }
    virtual bool Run(RayCastResult& item) const = 0;
};
class AllFilterItem : public FilterItem {
public:
    bool Run(RayCastResult& item) const override { return true; }
};
class FormTypeFilterItem : public FilterItem {
public:
    std::vector<RE::FormType> formType;
    bool Run(RayCastResult& item) const override {
        if (item.object) {
            if (auto base = item.object->GetBaseObject()) {
                auto targetType = base->GetFormType();
                for (auto& filterType : formType) {
                    if (filterType == targetType) {
                        return true;
                    }
                }
            }
        }
        return false;
    }
};
class FormIdFilterItem : public FilterItem {
public:
    std::vector<RE::FormID> formId;
    bool Run(RayCastResult& item) const override {
        if (item.object) {
            if (auto base = item.object->GetBaseObject()) {
                auto targetId = base->GetFormID();
                for (auto& filterId : formId) {
                    if (filterId == targetId) {
                        return true;
                    }
                }
            }
        }
        return false;
    }
};
class LandTextureFilter : public FilterItem {
public:
    std::vector<RE::MATERIAL_ID> materialIds;
    bool Run(RayCastResult& ray) const override {
        if (!ray.object) {
            if (auto texture = RE::TES::GetSingleton()->GetLandTexture(ray.position)) {
                if (auto material = texture->materialType) {
                    for (auto& materialId : materialIds) {
                        if (materialId == material->materialID) {
                            return true;
                        }
                    }
                }
            }
        }
        return false;
    }
};

struct FilterGroup {
    FilterItem* sourceFilter;
    FilterItem* targetFilter;
};

class ObjectReferenceFilter {
    std::vector<FilterGroup> filters;
    public:
    void AddLine(FilterItem* targetFilter, FilterItem* sourceFilter = nullptr);
    bool Match(RE::TESObjectREFR* source, RayCastResult& item);
};