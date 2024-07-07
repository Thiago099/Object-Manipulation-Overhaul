#include "Lib//ObjectReferenceFilter.h"
bool ObjectReferenceFilter::Match(RE::TESObjectREFR* source, RayCastResult& item) {
    bool isValid = false;

    auto sourceRay = RayCastResult({}, source);

    for (const auto& filter : filters) {

        if (source && filter.sourceFilter) {
            if (!filter.sourceFilter->Run(sourceRay)) {
                continue;
            }
        }

        auto filterResult = filter.targetFilter->Run(item);
        auto kind = filter.targetFilter->GetFilterKind();
        if (filterResult) {
            if (kind == FilterItem::Add) {
                isValid = true;
            } else if (kind == FilterItem::Remove) {
                isValid = false;
            }
        }
    }
    return isValid;
}
void ObjectReferenceFilter::AddLine(FilterItem* targetFilter, FilterItem* sourceFilter) { filters.push_back(FilterGroup(sourceFilter,targetFilter)); }
