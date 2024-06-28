#include "ObjectManipulationManager.h"

 void ObjectManipulationManager::Install() {
    auto builder = new HookBuilder();
    builder->AddCall<ProcessInputQueueHook, 5, 14>(67315, 0x7B, 68617, 0x7B, 0xC519E0, 0x81);
    builder->Install();
    delete builder;
    stateColorMap[ValidState::Valid] = Utils::CreateColor(0x00CCFFaa);
    stateColorMap[ValidState::Error] = Utils::CreateColor(0xFF0000aa);
}

void ObjectManipulationManager::StartDraggingObject(RE::TESObjectREFR* refr) {
    if (refr) {
        lastPos = refr->GetPosition();
        lastAngle = refr->GetAngle();
        auto [cameraAngle, cameraPosition] = Utils::GetCameraData();
        angleOffset = Utils::NormalizeAngle(refr->GetAngle().z -
                                            std::atan2(cameraPosition.x - lastPos.x, cameraPosition.y - lastPos.y));
        positionOffset = RE::NiPoint3(0, 0, 0);
        ctrlKey = false;
        pickObject = refr;
        currentState = ValidState::None;
        stateBuffer = ValidState::Valid;
        monitorState = MonitorState::Running;

        auto obj3d = refr->Get3D();
        colisionLayer = obj3d->GetCollisionLayer();
        if (Utils::IsStatic(colisionLayer)) {
            obj3d->SetCollisionLayer(RE::COL_LAYER::kNonCollidable);
        }
    }
}

void ObjectManipulationManager::CancelDrag() {
    monitorState = MonitorState::Idle;
    auto obj = pickObject;
    auto obj3d = obj->Get3D();
    auto color = RE::NiColorA(0, 0, 0, 0);
    ResetCollision();
    Utils::MoveTo_Impl(obj, RE::ObjectRefHandle(), obj->GetParentCell(), obj->GetWorldspace(), lastPos, lastAngle);
}

void ObjectManipulationManager::CommitDrag() {
    monitorState = MonitorState::Idle;
    auto obj = pickObject;
    auto obj3d = obj->Get3D();
    auto color = RE::NiColorA(0, 0, 0, 0);
    obj3d->TintScenegraph(color);
    if (Utils::IsStatic(colisionLayer)) {
        ResetCollision();
        obj->SetPosition(obj->GetPosition());
    }
}

void ObjectManipulationManager::Update() {
    auto obj = pickObject;

    if (!obj || !obj->Get3D()) {
        monitorState = MonitorState::Idle;
        return;
    }

    auto player3d = Utils::GetPlayer3d();

    auto pick3d = pickObject->Get3D();

    const auto evaluator = [player3d, pick3d](RE::NiAVObject* obj) {
        if (obj == player3d) {
            return false;
        }
        if (obj == pick3d) {
            return false;
        }
        return true;
    };

    auto [cameraPosition, rayPostion] = Utils::PlayerCameraRayPos(evaluator);

    UpdatePlaceholderPosition();

    Utils::SetPosition(obj, rayPostion + positionOffset);
    Utils::SetAngle(
        obj,
        RE::NiPoint3(0, 0, std::atan2(cameraPosition.x - rayPostion.x, cameraPosition.y - rayPostion.y) + angleOffset));

    obj->Update3DPosition(true);

    if (Utils::DistanceBetweenTwoPoints(cameraPosition, rayPostion) < 1000) {
        SetPlacementState(ValidState::Valid);
    } else {
        SetPlacementState(ValidState::Error);
    }

    // logger::info("Water: {}, name: {}", obj->IsInWater(),
    //                  RE::TES::GetSingleton()->GetLandTexture(rayPostion)->materialType->materialName);
}

void ObjectManipulationManager::Clean() {
    monitorState = MonitorState::Idle;
}

void ObjectManipulationManager::SetPlacementState(ValidState id) {
    if (id != currentState || currentState == ValidState::None) {
        currentState = id;
        auto obj = pickObject;
        auto color = stateColorMap[id];
        if (auto obj3d = obj->Get3D()) {
            obj3d->TintScenegraph(color);
        }
    }
}

void ObjectManipulationManager::ResetCollision() {
    if (auto obj3d = pickObject->Get3D()) {
        auto current = obj3d->GetCollisionLayer();
        if (current != colisionLayer) {
            obj3d->SetCollisionLayer(colisionLayer);
        }
    }
}

void ObjectManipulationManager::UpdatePlaceholderPosition() {
    auto player = RE::PlayerCharacter::GetSingleton();
    if (pickObject->GetWorldspace() != player->GetWorldspace() ||
        pickObject->GetParentCell() != player->GetParentCell()) {
        Utils::MoveTo_Impl(pickObject, RE::ObjectRefHandle(), player->GetParentCell(),
                    player->GetWorldspace(),
                    player->GetPosition(), pickObject->GetAngle());
        currentState = ValidState::None;
    }
}

bool ObjectManipulationManager::ProcessActiveInputState(RE::InputEvent* current) {
    bool suppress = false;
    if (auto button = current->AsButtonEvent()) {
        if (button->GetDevice() == RE::INPUT_DEVICE::kKeyboard) {
            switch (auto key = static_cast<RE::BSKeyboardDevice::Key>(button->GetIDCode())) {
                case RE::BSKeyboardDevice::Key::kLeftControl:
                case RE::BSKeyboardDevice::Key::kRightControl:
                    if (button->IsDown()) {
                        ctrlKey = true;
                    } else if (button->IsUp()) {
                        ctrlKey = false;
                    }
                    return true;
                default:
                    break;
            }
        }
        if (button->GetDevice() == RE::INPUT_DEVICE::kMouse) {
            switch (auto key = static_cast<RE::BSWin32MouseDevice::Key>(button->GetIDCode())) {
                case RE::BSWin32MouseDevice::Key::kWheelUp:
                    if (ctrlKey) {
                        positionOffset.z += 1.f;
                    } else {
                        angleOffset = Utils::NormalizeAngle(angleOffset + M_PI / 30);
                    }
                    return true;
                case RE::BSWin32MouseDevice::Key::kWheelDown:
                    if (ctrlKey) {
                        positionOffset.z -= 1.f;
                    } else {
                        angleOffset = Utils::NormalizeAngle(angleOffset - M_PI / 30);
                    }
                    return true;
                case RE::BSWin32MouseDevice::Key::kRightButton:
                    CancelDrag();
                    return true;
                case RE::BSWin32MouseDevice::Key::kLeftButton:
                    if (currentState == ValidState::Valid) {
                        CommitDrag();
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
                if (static_cast<RE::BSWin32MouseDevice::Key>(button->GetIDCode()) ==
                    RE::BSWin32MouseDevice::Key::kMiddleButton) {
                    auto player3d = Utils::GetPlayer3d();

                    const auto evaluator = [player3d](RE::NiAVObject* obj) {
                        if (obj == player3d) {
                            return false;
                        }
                        return true;
                    };
                    if (auto ref = Utils::PlayerCameraRayRefr(evaluator, 1000)) {
                        StartDraggingObject(ref);
                    }
                }
            }
        }
}
void ObjectManipulationManager::ProcessInputQueueHook::thunk(RE::BSTEventSource<RE::InputEvent*>* a_dispatcher,
                                                             RE::InputEvent* const* a_event) {
    if (monitorState != MonitorState::Idle) {
        Update();
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
