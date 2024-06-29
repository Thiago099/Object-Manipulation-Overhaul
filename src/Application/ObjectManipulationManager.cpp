#include "Application/ObjectManipulationManager.h"

 void ObjectManipulationManager::Install() {
    auto builder = new HookBuilder();
    builder->AddCall<Input::ProcessInputQueueHook, 5, 14>(67315, 0x7B, 68617, 0x7B, 0xC519E0, 0x81);
    builder->Install();
    delete builder;
    State::stateColorMap[State::ValidState::Valid] = Misc::CreateColor(0x00CCFFaa);
    State::stateColorMap[State::ValidState::Error] = Misc::CreateColor(0xFF0000aa);

    auto control = RE::ControlMap::GetSingleton();
    
    Input::activateKey =
        static_cast<RE::BSKeyboardDevice::Key>(control->GetMappedKey("Activate", RE::INPUT_DEVICE::kKeyboard));
 }

void ObjectManipulationManager::StartDraggingObject(RE::TESObjectREFR* refr) {
    if (refr) {
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

bool ObjectManipulationManager::ProcessActiveInputState(RE::InputEvent* current) {
    bool suppress = false;
    if (auto button = current->AsButtonEvent()) {
        if (button->GetDevice() == RE::INPUT_DEVICE::kKeyboard) {
            auto key = static_cast<RE::BSKeyboardDevice::Key>(button->GetIDCode());

            if (key == Input::activateKey) {
                return true;
            }

            switch (key) {
                case RE::BSKeyboardDevice::Key::kLeftControl:
                case RE::BSKeyboardDevice::Key::kRightControl:
                    if (button->IsDown()) {
                        Input::isControlKeyDown = true;
                    } else if (button->IsUp()) {
                        Input::isControlKeyDown = false;
                    }
                    return true;
                default:
                    break;
            }
        }
        if (button->GetDevice() == RE::INPUT_DEVICE::kMouse) {
            switch (auto key = static_cast<RE::BSWin32MouseDevice::Key>(button->GetIDCode())) {
                case RE::BSWin32MouseDevice::Key::kWheelUp:
                    if (Input::isControlKeyDown) {
                        Selection::positionOffset.z += 1.f;
                    } else {
                        Selection::angleOffset = Misc::NormalizeAngle(Selection::angleOffset + M_PI / 30);
                    }
                    return true;
                case RE::BSWin32MouseDevice::Key::kWheelDown:
                    if (Input::isControlKeyDown) {
                        Selection::positionOffset.z -= 1.f;
                    } else {
                        Selection::angleOffset = Misc::NormalizeAngle(Selection::angleOffset - M_PI / 30);
                    }
                    return true;
                case RE::BSWin32MouseDevice::Key::kRightButton:
                    if (button->IsDown()) {
                        CancelDrag();
                    }
                    return true;
                case RE::BSWin32MouseDevice::Key::kLeftButton:
                    if (button->IsDown()) {
                        if (State::validState == State::ValidState::Valid) {
                            CommitDrag();
                        }
                    }
                    return true;
                default:
                    break;
            }
        }
    }
    return false;
}
void ObjectManipulationManager::ProcessIdleInputState(RE::InputEvent * current) {
        if (auto button = current->AsButtonEvent()) {
            if (button->IsDown() && button->GetDevice() == RE::INPUT_DEVICE::kMouse) {
                auto mouseButton = static_cast<RE::BSWin32MouseDevice::Key>(button->GetIDCode());
                if (mouseButton == RE::BSWin32MouseDevice::Key::kMiddleButton && button->IsDown()) {
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
        }
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
            bool suppress = ProcessActiveInputState(current);
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
            ProcessIdleInputState(current);
        }

        originalFunction(a_dispatcher, a_event);
    }
}
