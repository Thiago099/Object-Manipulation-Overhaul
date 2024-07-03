#pragma once
#include <glm/ext.hpp>
#include <glm/glm.hpp>
#include "Lib/HookBuilder.h"
#include "Lib/Raycast.h"
#include "Lib/Misc.h"
#include "Lib/InputManager.h"
#include "Lib/ObjectReferenceFilter.h"

class ObjectManipulationManager {

    class Selection {
        public:
        static inline RE::TESObjectREFR* object;
        static inline RE::COL_LAYER objectOriginalCollisionLayer;

        static inline glm::vec2 rotateOffset = glm::vec2(0.f, 0.f);
        static inline glm::vec2 moveOffset = glm::vec2(0.f, 0.f);

        static inline RE::NiPoint3 lastPosition;
        static inline RE::NiPoint3 lastAngle;
        static inline void UpdateObjectTransform(RE::NiPoint3& rayPosition);
        static inline ObjectReferenceFilter* objectReferneceFilter = new ObjectReferenceFilter();
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
        static inline bool doToggleWithToggleKey = false;
        static inline bool isToggleRotateDown = false;
        static inline bool isToggleMoveDown = false;
        static inline bool IsAdvancedMode = false;
        static inline InputManager* passiveInputManager = new InputManager();
        static inline InputManager* activeInputManager = new InputManager();

        class PassiveState {
            public:
            static void Pick(RE::ButtonEvent* button);
        };
        class ActiveState {
            public:
            static inline bool blockState = false;
            static void ToggleRotate(RE::ButtonEvent* button);
            static void ToggleMove(RE::ButtonEvent* button);
            static void Cancel(RE::ButtonEvent* button);
            static void Commit(RE::ButtonEvent* button);
            static void ResetTransform(RE::ButtonEvent* button);
            static void AdvancedMode(RE::ButtonEvent* button);
            static bool ProcessMouseMovement(RE::MouseMoveEvent* move);
        };
    };


    static void SetPlacementState(State::ValidState id);
    static void ResetCollision();
    static void Update();
    static bool BlockActivateButton(RE::InputEvent* current);
    static bool UpdatePlaceholderPosition();
    static void TryInitialize();
    public:
    static InputManager* GetPassiveInputManager();
    static InputManager* GetActiveInputManager();
    static ObjectReferenceFilter* GetRaycastReferenceFilter();
    static void SetdoToggleWithToggleKey(bool value);
    static void Clean();
    static void Install();
    static void StartDraggingObject(RE::TESObjectREFR* obj);
    static void CancelDrag();
    static void CommitDrag();




};

