#include "Application/ObjectManipulationManager.h"

 void ObjectManipulationManager::Install() {
    auto builder = new HookBuilder();
    builder->AddCall<Input::ProcessInputQueueHook, 5, 14>(67315, 0x7B, 68617, 0x7B, 0xC519E0, 0x81);
    builder->Install();
    delete builder;
    State::stateColorMap[State::ValidState::Valid] = Misc::CreateColor(0x00CCFFaa);
    State::stateColorMap[State::ValidState::Error] = Misc::CreateColor(0xFF0000aa);

    Input::passiveInputManager->AddSink("Pick", Input::PassiveState::Pick);
    Input::activeInputManager->AddSink("ToggleMoveRotate", Input::ActiveState::ToggleMoveRotate);
    Input::activeInputManager->AddSink("TranslateLeft", Input::ActiveState::TranslateLeft);
    Input::activeInputManager->AddSink("TranslateRight", Input::ActiveState::TranslateRight);
    Input::activeInputManager->AddSink("Cancel", Input::ActiveState::Cancel);
    Input::activeInputManager->AddSink("Commit", Input::ActiveState::Commit);



 }

void ObjectManipulationManager::StartDraggingObject(RE::TESObjectREFR* refr) {
    if (refr) {
        CancelDrag();
        Selection::lastPosition = refr->GetPosition();
        Selection::lastAngle = refr->GetAngle();
        auto [cameraAngle, cameraPosition] = RayCast::GetCameraData();
        Selection::angleOffset = Misc::NormalizeAngle(refr->GetAngle().z - std::atan2(cameraPosition.x - Selection::lastPosition.x,
                                                                 cameraPosition.y - Selection::lastPosition.y));
        Selection::positionOffset = RE::NiPoint3(0, 0, 0);
        Input::isControlKeyDown = false;
        Selection::object = refr;
        State::validState = State::ValidState::None;
        State::dragState = State::DragState::Initializing;
    }
}

void ObjectManipulationManager::TryInitialize() {
    auto obj3d = Selection::object->Get3D();
    if (obj3d) {
        Selection::objectOriginalCollisionLayer = obj3d->GetCollisionLayer();
        if (Misc::IsStatic(Selection::objectOriginalCollisionLayer)) {
            obj3d->SetCollisionLayer(RE::COL_LAYER::kNonCollidable);
        }
        State::dragState = State::DragState::Running;
    }

}

void ObjectManipulationManager::CancelDrag() {
    if (State::dragState == State::DragState::Idle) {
        return;
    }
    State::dragState = State::DragState::Idle;
    auto obj = Selection::object;
    if (!obj) {
        return;
    }
    auto obj3d = obj->Get3D();
    if (obj3d) {
        auto color = RE::NiColorA(0, 0, 0, 0);
        obj3d->TintScenegraph(color);
    }
    if (Misc::IsStatic(Selection::objectOriginalCollisionLayer)) {
        ResetCollision();
        Misc::MoveTo_Impl(obj, RE::ObjectRefHandle(), obj->GetParentCell(), obj->GetWorldspace(),
                          Selection::lastPosition, Selection::lastAngle);
    } else {
        Misc::SetPosition(obj, Selection::lastPosition);
        Misc::SetAngle(obj, Selection::lastAngle);
        obj->Update3DPosition(true);
    }
}

void ObjectManipulationManager::CommitDrag() {
    State::dragState = State::DragState::Idle;
    auto obj = Selection::object;
    if (!obj) {
        return;
    }
    auto obj3d = obj->Get3D();
    if (obj3d) {
        auto color = RE::NiColorA(0, 0, 0, 0);
        obj3d->TintScenegraph(color);
    }
    if (Misc::IsStatic(Selection::objectOriginalCollisionLayer)) {
        ResetCollision();
        obj->SetPosition(obj->GetPosition());
    }
}

void ObjectManipulationManager::Update() {


    auto obj = Selection::object;

    if (!obj) {
        State::dragState = State::DragState::Idle;
        return;
    }

    if (UpdatePlaceholderPosition()) {
        return;
    }

    auto player3d = Misc::GetPlayer3d();

    auto pick3d = Selection::object->Get3D();

    const auto evaluator = [player3d, pick3d](RE::NiAVObject* obj) {
        if (obj == player3d) {
            return false;
        }
        if (obj == pick3d) {
            return false;
        }
        return true;
    };

    auto [cameraPosition, rayPostion] = RayCast::GetCursorPosition(evaluator);


    Misc::SetPosition(obj, rayPostion + Selection::positionOffset);
    Misc::SetAngle(
        obj, RE::NiPoint3(0, 0,
                                     std::atan2(cameraPosition.x - rayPostion.x, cameraPosition.y - rayPostion.y) +
                                         Selection::angleOffset));

    obj->Update3DPosition(true);

    if (Misc::DistanceBetweenTwoPoints(cameraPosition, rayPostion) < 1000) {
        SetPlacementState(State::ValidState::Valid);
    } else {
        SetPlacementState(State::ValidState::Error);
    }

    // logger::info("Water: {}, name: {}", obj->IsInWater(),
    //                  RE::TES::GetSingleton()->GetLandTexture(rayPostion)->materialType->materialName);
}

InputManager* ObjectManipulationManager::GetPassiveInputManager() { return Input::passiveInputManager; }

InputManager* ObjectManipulationManager::GetActiveInputManager() { return Input::activeInputManager; }

void ObjectManipulationManager::Clean() {
    State::dragState = State::DragState::Idle;
}

void ObjectManipulationManager::SetPlacementState(State::ValidState id) {
    if (id != State::validState || State::validState == State::ValidState::None) {
        auto obj = Selection::object;
        auto color = State::stateColorMap[id];
        if (auto obj3d = obj->Get3D()) {
            State::validState = id;
            obj3d->TintScenegraph(color);
        }
    }
}

void ObjectManipulationManager::ResetCollision() {
    if (auto obj3d = Selection::object->Get3D()) {
        auto current = obj3d->GetCollisionLayer();
        if (current != Selection::objectOriginalCollisionLayer) {
            obj3d->SetCollisionLayer(Selection::objectOriginalCollisionLayer);
        }
        State::validState = State::ValidState::None;
    }
}

bool ObjectManipulationManager::UpdatePlaceholderPosition() {
    auto player = RE::PlayerCharacter::GetSingleton();
    if (Selection::object->GetWorldspace() != player->GetWorldspace() ||
        Selection::object->GetParentCell() != player->GetParentCell()) {
        Misc::MoveTo_Impl(Selection::object, RE::ObjectRefHandle(), player->GetParentCell(),
                    player->GetWorldspace(), player->GetPosition(), Selection::object->GetAngle());
        State::validState = State::ValidState::None;
        State::dragState = State::DragState::Initializing;
        return true;
    }
    return false;
}

bool ObjectManipulationManager::BlockActivateButton(RE::InputEvent* current) {
    bool suppress = false;
    if (auto button = current->AsButtonEvent()) {
        if (button->GetDevice() == RE::INPUT_DEVICE::kKeyboard) {
            auto key = static_cast<RE::BSKeyboardDevice::Key>(button->GetIDCode());
            //https://ck.uesp.net/wiki/GetMappedKey_-_Input
            //Activate
            //Auto-Move
            //Back
            //CameraPath
            //Console
            //Favorites
            //Forward
            //Hotkey1
            //Hotkey2
            //Hotkey3
            //Hotkey4
            //Hotkey5
            //Hotkey6
            //Hotkey7
            //Hotkey8
            //Journal
            //Jump
            //Left Attack/Block
            //Look
            //Move
            //Multi-Screenshot
            //Pause
            //Quick Inventory
            //Quick Magic
            //Quick Map
            //Quick Stats
            //Quickload
            //Quicksave
            //Ready Weapon
            //Right Attack/Block
            //Run
            //Screenshot
            //Shout
            //Sneak
            //Sprint
            //Strafe Left
            //Strafe Right
            //Toggle Always Run
            //Toggle POV
            //Tween Menu
            //Wait
            //Zoom In
            //Zoom Out
            auto control = RE::ControlMap::GetSingleton();
            if (control) {
                if (key == static_cast<RE::BSKeyboardDevice::Key>(control->GetMappedKey("Activate", RE::INPUT_DEVICE::kKeyboard))) 
                {
                    return true;
                }
            }
        }
        if (button->GetDevice() == RE::INPUT_DEVICE::kGamepad) {
            auto key = static_cast<RE::BSWin32GamepadDevice::Key>(button->GetIDCode());
            auto control = RE::ControlMap::GetSingleton();
            if (control) {
                if (key == static_cast<RE::BSWin32GamepadDevice::Key>(control->GetMappedKey("Activate", RE::INPUT_DEVICE::kGamepad))) 
                {
                    return true;
                }
            }
        }
            auto key = static_cast<RE::BSKeyboardDevice::Key>(button->GetIDCode());
    }
    return false;
}

void ObjectManipulationManager::Input::ProcessInputQueueHook::thunk(RE::BSTEventSource<RE::InputEvent*>* a_dispatcher,
                                                             RE::InputEvent* const* a_event) {
    if (State::dragState != State::DragState::Idle) {
        if (State::dragState == State::DragState::Initializing) {
            TryInitialize();
        }

        if (State::dragState == State::DragState::Running) {
            Update();
        }

        auto first = *a_event;
        auto last = *a_event;
        size_t length = 0;
        for (auto current = *a_event; current; current = current->next) {

            bool suppress = BlockActivateButton(current);
            if (auto button = current->AsButtonEvent()) {
                if (Input::activeInputManager->ProcessInput(button)) {
                    suppress = true;
                }
            }
            if (suppress) {
                if (current != last) {
                    last->next = current->next;
                } else {
                    last = current->next;
                    first = current->next;
                }
            } else {
                last = current;
                ++length;
            }
        }
        if (length == 0) {
            constexpr RE::InputEvent* const dummy[] = {nullptr};
            originalFunction(a_dispatcher, dummy);
        } else {
            RE::InputEvent* const e[] = {first};
            originalFunction(a_dispatcher, e);
        }
    } else {
        for (auto current = *a_event; current; current = current->next) {
            if (auto button = current->AsButtonEvent()) {
                Input::passiveInputManager->ProcessInput(button);
            }
        }

        originalFunction(a_dispatcher, a_event);
    }
}

void ObjectManipulationManager::Input::PassiveState::Pick(RE::ButtonEvent* button) {
    if (button->IsDown()) {
        auto player3d = Misc::GetPlayer3d();
        const auto evaluator = [player3d](RE::NiAVObject* obj) {
            if (obj == player3d) {
                return false;
            }
            return true;
        };
        if (auto ref = RayCast::GetObjectAtCursor(evaluator, 1000)) {
            if (ObjectReferenceFilter::Match(ref)) {
                StartDraggingObject(ref);
            }
        }
    }
}


void ObjectManipulationManager::Input::ActiveState::ToggleMoveRotate(RE::ButtonEvent* button) {
    if (button->IsDown()) {
        Input::isControlKeyDown = true;
    } else if (button->IsUp()) {
        Input::isControlKeyDown = false;
    }
}

void ObjectManipulationManager::Input::ActiveState::TranslateLeft(RE::ButtonEvent* button) {
    if (Input::isControlKeyDown) {
        Selection::positionOffset.z += 1.f;
    } else {
        Selection::angleOffset = Misc::NormalizeAngle(Selection::angleOffset + M_PI / 30);
    }
}

void ObjectManipulationManager::Input::ActiveState::TranslateRight(RE::ButtonEvent* button) {
    if (Input::isControlKeyDown) {
        Selection::positionOffset.z -= 1.f;
    } else {
        Selection::angleOffset = Misc::NormalizeAngle(Selection::angleOffset - M_PI / 30);
    }
}

void ObjectManipulationManager::Input::ActiveState::Cancel(RE::ButtonEvent* button) {
    if (button->IsDown()) {
        CancelDrag();
    }
}

void ObjectManipulationManager::Input::ActiveState::Commit(RE::ButtonEvent* button) {
    if (button->IsDown()) {
        if (State::validState == State::ValidState::Valid) {
            CommitDrag();
        }
    }
}
