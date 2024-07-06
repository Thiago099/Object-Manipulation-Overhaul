#include "Lib//ObjectReferenceFilter.h"
bool ObjectReferenceFilter::Match(RayCastResult& item) {
    bool isValid = false;
    for (const auto& filter : filters) {
        auto filterResult = filter->Run(item);
        auto kind = filter->GetFilterKind();
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
void ObjectReferenceFilter::AddLine(FilterItem* item) { filters.push_back(item); }
