#pragma once

#include <windows.h>

namespace ObjectManipulationOverhaul {
    inline void StartDraggingObject(RE::TESObjectREFR* ref) {
        using func_t = void(*)(RE::TESObjectREFR*);
        static auto ObjectManipulationOverhaul = GetModuleHandle(L"ObjectManipulationOverhaul");
        func_t func = reinterpret_cast<func_t>(GetProcAddress(ObjectManipulationOverhaul, "StartDraggingObject"));
        return func(ref);
    }
    inline RE::TESObjectREFR* PlaceObject(RE::TESObjectREFR* location, RE::TESBoundObject* obj, bool forcePersist = false) {
        using func_t = RE::TESObjectREFR*(*)(RE::TESObjectREFR* location, RE::TESBoundObject* obj, bool forcePersist);
        static auto ObjectManipulationOverhaul = GetModuleHandle(L"ObjectManipulationOverhaul");
        func_t func = reinterpret_cast<func_t>(GetProcAddress(ObjectManipulationOverhaul, "PlayerPlaceObject"));
        return func(location,obj, forcePersist);
    }
}