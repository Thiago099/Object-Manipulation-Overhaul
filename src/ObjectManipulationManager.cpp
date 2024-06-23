#include "ObjectManipulationManager.h"


void MoveTo_Impl(RE::TESObjectREFR* ref, const RE::ObjectRefHandle& a_targetHandle, RE::TESObjectCELL* a_targetCell,
                 RE::TESWorldSpace* a_selfWorldSpace, const RE::NiPoint3& a_position, const RE::NiPoint3& a_rotation) {
    using func_t = decltype(&MoveTo_Impl);
    REL::Relocation<func_t> func{RE::Offset::TESObjectREFR::MoveTo};
    return func(ref, a_targetHandle, a_targetCell, a_selfWorldSpace, a_position, a_rotation);
}


void ObjectManipulationManager::SetPlacementState(ValidState id) {
    if (id != currentState) {
        currentState = id;
        if (currentState == ValidState::Valid) {
            Utils::CallPapyrusAction(placeholderRef, "OM_MarkerScript", "SetValid");
        
        } else if (currentState == ValidState::Error) {
            Utils::CallPapyrusAction(placeholderRef, "OM_MarkerScript", "SetError");
        }
    }
}

void ObjectManipulationManager::CreatePlaceholder() {

    auto player = RE::PlayerCharacter::GetSingleton();
    placeholderRef = RE::TESDataHandler::GetSingleton()
                         ->CreateReferenceAtLocation(placeholder, player->GetPosition(), RE::NiPoint3(0, 0, 0),
                                                     player->GetParentCell(), player->GetWorldspace(), nullptr, nullptr,
                                                     RE::ObjectRefHandle(), true, true)
                         .get()
                         .get();

    Utils::CallPapyrusAction(placeholderRef, "OM_MarkerScript", "SetValid");

}


RE::TESEffectShader* LookUpShader(uint32_t id) {
    const auto dataHandler = RE::TESDataHandler::GetSingleton();
    auto shaderFormId = dataHandler->LookupFormID(id, "ObjectManipulator.esp");
    return RE::TESForm::LookupByID<RE::TESEffectShader>(shaderFormId);
}

void ObjectManipulationManager::Install() {
    auto builder = new HookBuilder();
    builder->AddCall<CameraHook, 5, 14>(49852, 0x50, 50784, 0x54);
    builder->AddCall<ProcessInputQueueHook, 5, 14>(67315, 0x7B, 68617, 0x7B, 0xC519E0, 0x81);
    builder->Install();

    const auto dataHandler = RE::TESDataHandler::GetSingleton();
    auto placeholderFormId = dataHandler->LookupFormID(0x803, "ObjectManipulator.esp");
    placeholder = RE::TESForm::LookupByID<RE::TESObjectACTI>(placeholderFormId);

    delete builder;
}


void ObjectManipulationManager::Pick(RE::TESObjectREFR* baseObject) {
    if (baseObject) {
        auto baseObj = baseObject->GetBaseObject();
        if (!baseObj) {
            return;
        }
        auto modelBase = baseObj->As<RE::TESModel>();
        if (!modelBase) {
            return;
        }
        placeholder->SetModel(modelBase->GetModel());
        CreatePlaceholder();
        if (!placeholderRef) {
            return;
        }
        angleOffset = M_PI;
        pickObject = baseObject;
        monitorState = MonitorState::Running;
        pickObject->Disable();
        auto obj = pickObject;

    }
}

void ObjectManipulationManager::Cancel() {
    if (placeholderRef) {
        monitorState = MonitorState::Idle;
        currentState = ValidState::None;
        Utils::CallPapyrusAction(placeholderRef, "OM_MarkerScript", "Destroy");
        pickObject->Enable(false);
    }
}
void ObjectManipulationManager::Release() {
    if (placeholderRef) {
        monitorState = MonitorState::Idle;
        currentState = ValidState::None;
        pickObject->Enable(false);
        pickObject->MoveTo(placeholderRef);
        Utils::CallPapyrusAction(placeholderRef, "OM_MarkerScript", "Destroy");

    }

}

void ObjectManipulationManager::UpdatePlaceholderPosition() {
    auto player = RE::PlayerCharacter::GetSingleton();
    if (placeholderRef->GetWorldspace() != player->GetWorldspace() ||
        placeholderRef->GetParentCell() != player->GetParentCell()) {
        MoveTo_Impl(placeholderRef, player->GetHandle(), player->GetParentCell(), player->GetWorldspace(),
                    player->GetPosition(), placeholderRef->GetAngle());
    }
}

void ObjectManipulationManager::Update() {
    auto obj = placeholderRef;

    auto obj3d = placeholderRef->Get3D();
    if (obj3d && obj3d->GetCollisionLayer() != RE::COL_LAYER::kNonCollidable) {
        obj3d->SetCollisionLayer(RE::COL_LAYER::kNonCollidable);
    }

    SKSE::GetTaskInterface()->AddTask([obj]() {
        auto [cameraPosition, rayPostion] = Utils::PlayerCameraRayPos();

        UpdatePlaceholderPosition();
        Utils::SetPosition(obj, rayPostion);
        Utils::SetAngle(
            obj, RE::NiPoint3(
                        0, 0, std::atan2(cameraPosition.x - rayPostion.x, cameraPosition.y - rayPostion.y) + angleOffset));
        obj->Update3DPosition(true);

        if (Utils::DistanceBetweenTwoPoints(cameraPosition, rayPostion) < 1000) {
            SetPlacementState(ValidState::Valid);
        } else {
            SetPlacementState(ValidState::Error);
        }
    });
}


void  ObjectManipulationManager::CameraHook::thunk(void* a1, RE::TESObjectREFR** refPtr) {
    originalFunction(a1, refPtr);
}

double normalizeAngle(double angle_rad) {
    // Normalize angle to be within [0, 2*pi)
    while (angle_rad < 0) {
        angle_rad += 2 * M_PI;
    }
    while (angle_rad >= 2 * M_PI) {
        angle_rad -= 2 * M_PI;
    }
    return angle_rad;
}

void ObjectManipulationManager::ProcessInputQueueHook::thunk(RE::BSTEventSource<RE::InputEvent*>* a_dispatcher,
                                                             RE::InputEvent* const* a_event) {
    if (monitorState != MonitorState::Idle) {
        Update();


        auto first = *a_event;
        auto last = *a_event;
        size_t length = 0;
        for (auto current = *a_event; current; current = current->next) {
            bool suppress = false;
            if (auto button = current->AsButtonEvent()) {

                if (button->GetDevice() == RE::INPUT_DEVICE::kMouse) {


                    switch (auto key = static_cast<RE::BSWin32MouseDevice::Key>(button->GetIDCode())) {
                        case RE::BSWin32MouseDevice::Key::kWheelUp:
                            suppress = true;
                            angleOffset = normalizeAngle(angleOffset + M_PI / 30);
                            break;
                        case RE::BSWin32MouseDevice::Key::kWheelDown:
                            suppress = true;
                            angleOffset = normalizeAngle(angleOffset - M_PI / 30);
                            break;
                        default:
                            if (RE::BSWin32MouseDevice::Key::kRightButton == key) {
                                suppress = true;
                                Cancel();
                            }
                            else if (RE::BSWin32MouseDevice::Key::kLeftButton == key) {
                                suppress = true;
                                if (currentState == ValidState::Valid) {
                                    Release();
                                }
                            }
                            break;
                    }
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
                if (button->IsDown() && button->GetDevice() == RE::INPUT_DEVICE::kMouse) {
                    if (static_cast<RE::BSWin32MouseDevice::Key>(button->GetIDCode()) ==
                        RE::BSWin32MouseDevice::Key::kMiddleButton) {
                        SKSE::GetTaskInterface()->AddTask([]() { 
                        
                            if (auto ref = Utils::PlayerCameraRayRefr()) {
                                Pick(ref);
                            }
                        });

                    }
                }

            }


        }

        originalFunction(a_dispatcher, a_event);
    }




}
