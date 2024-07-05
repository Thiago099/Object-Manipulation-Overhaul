#include "Lib//ObjectReferenceFilter.h"
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
void ObjectReferenceFilter::AddLine(FilterItem* item) { filters.push_back(item); }
