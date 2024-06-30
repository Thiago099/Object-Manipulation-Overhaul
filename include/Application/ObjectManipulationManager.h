#pragma once
#include "Lib/HookBuilder.h"
#include "Lib/Raycast.h"
#include "Lib/Misc.h"
#include "Lib/InputManager.h"
#include "Application/ObjectReferenceFilter.h"
#define M_PI 3.14159265358979323846

class ObjectManipulationManager {

    class Selection {
        public:
        static inline RE::TESObjectREFR* object;
        static inline RE::COL_LAYER objectOriginalCollisionLayer;

        static inline float angleOffset = M_PI;
        static inline RE::NiPoint3 positionOffset;

        static inline RE::NiPoint3 lastPosition;
        static inline RE::NiPoint3 lastAngle;
    };

    class State {
        public:
        enum ValidState {
            None,
            Valid,
            Error,
            Warn,
        };

        enum DragState { Idle, Initializing, Running };
        static inline ValidState validState = ValidState::None;
        static inline DragState dragState = DragState::Idle;
        static inline std::map<State::ValidState, RE::NiColorA> stateColorMap;

    };
    class Input {
        public:
        struct ProcessInputQueueHook {
            static void thunk(RE::BSTEventSource<RE::InputEvent*>* a_dispatcher, RE::InputEvent* const* a_event);
            static inline REL::Relocation<decltype(thunk)> originalFunction;
        };
        static inline bool isControlKeyDown = false;
        static inline InputManager* passiveInputManager = new InputManager();
        static inline InputManager* activeInputManager = new InputManager();

        class PassiveState {
            public:
            static void Pick(RE::ButtonEvent* button);
        };
        class ActiveState {
            public:
            static inline bool blockState = false;
            static void ToggleMoveRotate(RE::ButtonEvent* button);
            static void TranslatePlus(RE::ButtonEvent* button);
            static void TranslateMinus(RE::ButtonEvent* button);
            static void Cancel(RE::ButtonEvent* button);
            static void Commit(RE::ButtonEvent* button);
        };
    };


    static void SetPlacementState(State::ValidState id);
    static void ResetCollision();
    static void Update();
    static bool BlockActivateButton(RE::InputEvent* current);
    static bool UpdatePlaceholderPosition();
    static void TryInitialize();
    public:

    static void Clean();
    static void Install();
    static void StartDraggingObject(RE::TESObjectREFR* obj);
    static void CancelDrag();
    static void CommitDrag();




};

