#pragma once
#include "Utils.h"
#include "HookBuilder.h"
#define M_PI 3.14159265358979323846

class ObjectManipulationManager {
    static inline RE::TESObjectREFR* pickedObject;
    static inline RE::ShaderReferenceEffect* shaderRef;
    
    struct CameraHook {
        static void thunk(void*, RE::TESObjectREFR** refPtr);
        static inline REL::Relocation<decltype(thunk)> originalFunction;
    };

    public:

   static void Install();

    static void Pick(RE::TESObjectREFR* obj);
    static void Release(RE::TESObjectREFR* obj);


};

