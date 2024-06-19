#pragma once
#include "Utils.h"
#include "HookBuilder.h"
#define M_PI 3.14159265358979323846

class ObjectManipulationManager {

    enum State {
        None,
        Valid,
        Error,
        Warn
    };

    static inline std::map<State, RE::TESEffectShader*> shaders;

    static inline RE::TESObjectREFR* pickedObject;
    static inline RE::ShaderReferenceEffect* shaderRef;
    static inline State currentState = State::None;
    static inline bool pickedObjectHasCollison = false;

    static void ApplyShader(State id);

    static void CleanShader();
    
    struct CameraHook {
        static void thunk(void*, RE::TESObjectREFR** refPtr);
        static inline REL::Relocation<decltype(thunk)> originalFunction;
    };

    public:

   static void Install();

    static void Pick(RE::TESObjectREFR* obj);
    static void Release();


};

