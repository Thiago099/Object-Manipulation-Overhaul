
#pragma once
#include "Lib/Misc.h"


class FilterItem {
public:
    bool value;
    bool GetFilterKind() const { return value; }
    virtual bool Run(RE::TESObjectREFR* item) const = 0;
};
class AllFilterItem : public FilterItem {
public:
    bool Run(RE::TESObjectREFR* item) const override { return true; }
};
class FormTypeFilterItem : public FilterItem {
public:
    RE::FormType formType;
    bool Run(RE::TESObjectREFR* item) const override {
        if (auto base = item->GetBaseObject()) {
            return base->GetFormType() == formType;
        }
        return false;
    }
};
class FormIdFilterItem : public FilterItem {
public:
    RE::FormID formId;
    bool Run(RE::TESObjectREFR* item) const override {
        if (auto base = item->GetBaseObject()) {
            return base->GetFormID() == formId;
        }
        return false;
    }
};

class ObjectReferenceFilter {
    std::vector<FilterItem*> filters;
    public:
    void AddLine(FilterItem* item);
    bool Match(RE::TESObjectREFR* item);
};