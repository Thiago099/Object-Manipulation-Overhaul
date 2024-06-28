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

    static inline RE::COL_LAYER colisionLayer;
    static inline RE::TESObjectREFR* pickObject;
    static inline ValidState stateBuffer = ValidState::None;
    static inline ValidState currentState = ValidState::None;
    static inline MonitorState monitorState = MonitorState::Idle;
    static inline bool pickedObjectHasCollison = false;

    static inline RE::NiPoint3 lastPos;
    static inline RE::NiPoint3 lastAngle;

    static inline bool ctrlKey = false;

    static inline std::map<ValidState, RE::NiColorA> stateColorMap;



    static inline float angleOffset = M_PI;
    static inline RE::NiPoint3 positionOffset;
    static void SetPlacementState(ValidState id);
    struct ProcessInputQueueHook {
        static void thunk(RE::BSTEventSource<RE::InputEvent*>* a_dispatcher, RE::InputEvent* const* a_event);
        static inline REL::Relocation<decltype(thunk)> originalFunction;
    };

    static void ResetCollision();
    static void Update();
    static bool ProcessActiveInputState(RE::InputEvent* current);
    static void ProcessIdleInputState(RE::InputEvent* current);
    static void UpdatePlaceholderPosition();

    public:

    static void Clean();
    static void Install();
    static void StartDraggingObject(RE::TESObjectREFR* obj);
    static void CancelDrag();
    static void CommitDrag();




};

