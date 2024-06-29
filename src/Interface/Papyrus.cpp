#include "Interface/Papyrus.h"



std::mutex papyrusMutex;

void StartDraggingObject(RE::StaticFunctionTag*, RE::TESObjectREFR* object) {
    std::lock_guard<std::mutex> lock(papyrusMutex);
    if (!object) {
        return;
    }
    ObjectManipulationManager::StartDraggingObject(object);
}
RE::TESObjectREFR* PlayerPlaceObject(RE::StaticFunctionTag*, RE::TESObjectREFR* location, RE::TESForm* object,
                                        bool forcePersist = false) {
    std::lock_guard<std::mutex> lock(papyrusMutex);
    if (!location || !object) {
        return nullptr;
    }
    
    if (auto bound = object->As<RE::TESBoundObject>()) {
        if (auto refptr = location->PlaceObjectAtMe(bound, forcePersist)) {
            if (auto ref = refptr.get()) {
                ObjectManipulationManager::StartDraggingObject(ref);
                return ref;
            }
        }
    }
    return nullptr;
}

bool Papyrus::PapyrusFunctions(RE::BSScript::IVirtualMachine* vm) {
    vm->RegisterFunction("StartDraggingObject", "ObjectManipulationOverhaul", StartDraggingObject);
    vm->RegisterFunction("PlayerPlaceObject", "ObjectManipulationOverhaul", PlayerPlaceObject);
    return true;
}

void Papyrus::Install() { 
	SKSE::GetPapyrusInterface()->Register(PapyrusFunctions);
}


