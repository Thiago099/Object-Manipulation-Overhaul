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

    static inline bool cdset = false;


    static inline RE::NiPoint3 cd = {};


    static inline std::map<ValidState, RE::TESEffectShader*> shaders;

    static inline RE::COL_LAYER colisionLayer;
    static inline RE::TESObjectREFR* pickObject;
    static inline ValidState stateBuffer = ValidState::None;
    static inline ValidState currentState = ValidState::None;
    static inline MonitorState monitorState = MonitorState::Idle;
    static inline bool pickedObjectHasCollison = false;

    static inline RE::NiPoint3 lastPos;
    static inline RE::NiPoint3 lastAngle;

    static inline bool ctrlKey = false;


    static inline float angleOffset = M_PI;
    static inline RE::NiPoint3 positionOffset;

    static void SetPlacementState(ValidState id);

    static void CreatePlaceholder();

    struct ProcessInputQueueHook {
        static void thunk(RE::BSTEventSource<RE::InputEvent*>* a_dispatcher, RE::InputEvent* const* a_event);
        static inline REL::Relocation<decltype(thunk)> originalFunction;
    };

    public:

   static void Install();

   static void Pick(RE::TESObjectREFR* obj);
   static void Cancel();
   static void Release();

   static void UpdatePlaceholderPosition();

   static void Update();


};

