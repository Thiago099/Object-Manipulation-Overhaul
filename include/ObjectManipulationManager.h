#pragma once
#include "Utils.h"
#include "HookBuilder.h"
#define M_PI 3.14159265358979323846

class ObjectManipulationManager {

    enum State {
        Idle,
        Active = 1 << 0,
        Valid = 1 << 1,
        Error = 1 << 2,
        Warn = 1 << 3
    };

    static inline std::map<State, RE::TESEffectShader*> shaders;

    static inline RE::TESObjectREFR* pickedObject;
    static inline RE::ShaderReferenceEffect* shaderRef;
    static inline State currentState = State::Idle;
    static inline bool pickedObjectHasCollison = false;

    static void ApplyShader(State id);

    static void CleanShader();
    
    struct CameraHook {
        static void thunk(void*, RE::TESObjectREFR** refPtr);
        static inline REL::Relocation<decltype(thunk)> originalFunction;
    };
    struct ProcessInputQueueHook {
        static void thunk(RE::BSTEventSource<RE::InputEvent*>* a_dispatcher, RE::InputEvent* const* a_event);
        static inline REL::Relocation<decltype(thunk)> originalFunction;
    };

    public:

   static void Install();

    static void Pick(RE::TESObjectREFR* obj);
    static void Release();


};

