#include "Interface/Papyrus.h"



std::mutex papyrusMutex;

void StartDraggingObject(RE::StaticFunctionTag*, RE::TESObjectREFR* object) {
    std::lock_guard<std::mutex> lock(papyrusMutex);
    if (!object) {
        return;
    }
    ObjectManipulationManager::StartDraggingObject(object);
}

bool Papyrus::PapyrusFunctions(RE::BSScript::IVirtualMachine* vm) {
    vm->RegisterFunction("StartDraggingObject", "ObjectManipulationOverhaul", StartDraggingObject);
    return true;
}

void Papyrus::Install() { 
	SKSE::GetPapyrusInterface()->Register(PapyrusFunctions);
}


