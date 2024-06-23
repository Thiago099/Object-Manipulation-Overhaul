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

    static inline RE::TESObjectREFR* placeholderRef;
    static inline RE::TESObjectACTI* placeholder;

    static inline RE::TESForm* pickObject;
    static inline ValidState currentState = ValidState::None;
    static inline MonitorState monitorState = MonitorState::Idle;
    static inline bool pickedObjectHasCollison = false;

    static inline float angleOffset = M_PI;

    static void SetPlacementState(ValidState id);

    static void CreatePlaceholder();

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

   static void Pick(RE::TESForm* obj);
   static void Cancel();
   static void Release();

   static void UpdatePlaceholderPosition();


};

