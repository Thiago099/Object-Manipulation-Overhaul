#include "ObjectManipulationManager.h"


void MoveTo_Impl(RE::TESObjectREFR* ref, const RE::ObjectRefHandle& a_targetHandle, RE::TESObjectCELL* a_targetCell,
                 RE::TESWorldSpace* a_selfWorldSpace, const RE::NiPoint3& a_position, const RE::NiPoint3& a_rotation) {
    using func_t = decltype(&MoveTo_Impl);
    REL::Relocation<func_t> func{RE::Offset::TESObjectREFR::MoveTo};
    return func(ref, a_targetHandle, a_targetCell, a_selfWorldSpace, a_position, a_rotation);
}

RE::NiPoint3 GetPlayerDummyPosition() {
    auto player = RE::PlayerCharacter::GetSingleton();
    return RE::NiPoint3(player->GetPositionX(), player->GetPositionY(), std::numeric_limits<float>::max());
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
    const auto dataHandler = RE::TESDataHandler::GetSingleton();
    auto placeholderFormId = dataHandler->LookupFormID(0x803, "ObjectManipulator.esp");
    placeholder = RE::TESForm::LookupByID<RE::TESObjectACTI>(placeholderFormId);
    auto player = RE::PlayerCharacter::GetSingleton();
    placeholderRef = RE::TESDataHandler::GetSingleton()
                         ->CreateReferenceAtLocation(placeholder, GetPlayerDummyPosition(), RE::NiPoint3(0, 0, 0),
                                                     player->GetParentCell(), player->GetWorldspace(), nullptr, nullptr,
                                                     RE::ObjectRefHandle(), true, true)
                         .get()
                         .get();
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
    delete builder;
}


void ObjectManipulationManager::Pick(RE::TESForm* baseObject) {
    if (baseObject) {
        CreatePlaceholder();
        if (!placeholderRef) {
            return;
        }
        pickObject = baseObject;
        placeholder->SetModel(baseObject->As<RE::TESModel>()->GetModel());
        monitorState = MonitorState::Running;
    }

}

void ObjectManipulationManager::Cancel() {
    if (placeholderRef) {
        monitorState = MonitorState::Idle;
        currentState = ValidState::None;
        Utils::CallPapyrusAction(placeholderRef, "OM_MarkerScript", "Destroy");
    }
}
void ObjectManipulationManager::Release() {
    if (placeholderRef) {
        monitorState = MonitorState::Idle;
        currentState = ValidState::None;
        placeholderRef->PlaceObjectAtMe(pickObject->As<RE::TESBoundObject>(), true);
        Utils::CallPapyrusAction(placeholderRef, "OM_MarkerScript", "Destroy");

    }

}

void ObjectManipulationManager::UpdatePlaceholderPosition() {
    auto player = RE::PlayerCharacter::GetSingleton();
    if (placeholderRef->GetWorldspace() != player->GetWorldspace() ||
        placeholderRef->GetParentCell() != player->GetParentCell()) {
        MoveTo_Impl(placeholderRef, player->GetHandle(), player->GetParentCell(), player->GetWorldspace(),
                    GetPlayerDummyPosition(), placeholderRef->GetAngle());
    }
}


void  ObjectManipulationManager::CameraHook::thunk(void*, RE::TESObjectREFR** refPtr) {
    if (monitorState != MonitorState::Idle) {

            auto obj = placeholderRef;

            RE::PlayerCamera* camera = RE::PlayerCamera::GetSingleton();
            auto thirdPerson =
                reinterpret_cast<RE::ThirdPersonState*>(camera->cameraStates[RE::CameraState::kThirdPerson].get());
            auto firstPerson =
                reinterpret_cast<RE::FirstPersonState*>(camera->cameraStates[RE::CameraState::kFirstPerson].get());

            RE::NiQuaternion rotation;
            if (camera->currentState.get()->id == RE::CameraState::kFirstPerson) {
                firstPerson->GetRotation(rotation);
            }
            if (camera->currentState.get()->id == RE::CameraState::kThirdPerson) {
                rotation = thirdPerson->rotation;
            }
            auto player = RE::PlayerCharacter::GetSingleton();

            auto pos = Utils::Raycast(player, rotation, camera->pos);

            
            UpdatePlaceholderPosition();


            SKSE::GetTaskInterface()->AddTask([obj,pos,camera]() {
                Utils::SetPosition(obj, pos);
                Utils::SetAngle(obj,
                         RE::NiPoint3(0, 0, std::atan2(camera->pos.x - pos.x, camera->pos.y - pos.y) + M_PI));
                obj->Update3DPosition(true);
            });

            if (Utils::DistanceBetweenTwoPoints(camera->pos, pos) < 1000) {
                SetPlacementState(ValidState::Valid);
            } else {
                SetPlacementState(ValidState::Error);
            }
        }
}

void ObjectManipulationManager::ProcessInputQueueHook::thunk(RE::BSTEventSource<RE::InputEvent*>* a_dispatcher,
                                                             RE::InputEvent* const* a_event) {
    if (currentState != MonitorState::Idle) {

        auto first = *a_event;
        auto last = *a_event;
        size_t length = 0;
        for (auto current = *a_event; current; current = current->next) {
            bool suppress = false;
            if (auto button = current->AsButtonEvent()) {

                if (button->GetDevice() == RE::INPUT_DEVICE::kMouse) {
                    switch (auto key = static_cast<RE::BSWin32MouseDevice::Key>(button->GetIDCode())) {
                        case RE::BSWin32MouseDevice::Key::kWheelUp:
                        case RE::BSWin32MouseDevice::Key::kWheelDown:
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
                    last->next = current;
                } else {
                    last = current;
                    first = current;
                }
            } else {
                ++length;
            }
            last = current;
        }
        if (length == 0) {
            constexpr RE::InputEvent* const dummy[] = {nullptr};
            originalFunction(a_dispatcher, dummy);
        } else {
            size_t i = 0;
            RE::InputEvent** result = new RE::InputEvent*[length];
            for (auto current = first; current; current = current->next) {
                result[i] = current;
                ++i;
            }
            originalFunction(a_dispatcher, const_cast<RE::InputEvent* const*>(result));
        }
    } else {
        originalFunction(a_dispatcher, a_event);
    }




}
