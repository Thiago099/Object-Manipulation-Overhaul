#pragma once
#include "Lib/Regex.h"
#include "Lib/File.h"
#include "Lib/Misc.h"

struct Item {
    float priority;
    bool value;
    std::string type;
    std::vector<std::string> props;
};

class FilterItem {
    public:
    bool value;
    bool GetFilterKind() const { return value; }
    virtual bool Run(RE::TESObjectREFR* item) const = 0;
};
class AllFilterItem : public FilterItem {
    public:
    bool Run(RE::TESObjectREFR* item) const override{
        return true;
    }
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
        bool Run(RE::TESObjectREFR* item) const override { return item->GetFormID() == formId; }
};
class ObjectReferenceFilter {
    static inline std::vector<FilterItem*> filters;
    static inline Regex bodyRegex = Regex("^\\s*([-+])\\s*([^\\s]*)\\s*\\(\\s*([^)]*)\\s*\\)$");
    static inline Regex parametersRegex = Regex("\\s*([^,\\s]+)\\s*(?:,|$)");
    static void AddLine(Item& item);
    public:
    static bool Match(RE::TESObjectREFR* item);
    static ObjectReferenceFilter& Install(std::string path, std::string regex);
};
