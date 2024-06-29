#include "Interface/ObjectManipulationOverhaul.h"
void StartDraggingObject(RE::TESObjectREFR* object) {
    if (!object) {
        return;
    }
	ObjectManipulationManager::StartDraggingObject(object); 
}

RE::TESObjectREFR* PlayerPlaceObject(RE::TESObjectREFR* location, RE::TESBoundObject* object,
                                                     bool forcePersist) {
    if (!location || !object) {
        return nullptr;
    }

    if (auto refptr = location->PlaceObjectAtMe(object, forcePersist)) {
        if (auto ref = refptr.get()) {
            ObjectManipulationManager::StartDraggingObject(ref);
            return ref;
        }
    }
    return nullptr;
}
