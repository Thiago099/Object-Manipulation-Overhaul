#pragma once
#include "Utils.h"
#include "HookBuilder.h"
#define M_PI 3.14159265358979323846

class ObjectManipulationManager {

    enum ValidState {
        None,
        Valid,
        Error,
        Warn,
    };

    enum MonitorState {
        Idle,
        Running
    };

    static inline std::map<ValidState, RE::TESEffectShader*> shaders;

    static inline RE::TESObjectREFR* pickedObject;
    static inline ValidState currentState = ValidState::None;
    static inline MonitorState monitorState = MonitorState::Idle;
    static inline bool pickedObjectHasCollison = false;

    static void SetPlacementState(ValidState id);

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

