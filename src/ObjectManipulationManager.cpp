#include "ObjectManipulationManager.h"

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

void MoveTo_Impl(RE::TESObjectREFR* ref, const RE::ObjectRefHandle& a_targetHandle, RE::TESObjectCELL* a_targetCell,
                 RE::TESWorldSpace* a_selfWorldSpace, const RE::NiPoint3& a_position, const RE::NiPoint3& a_rotation) {
    using func_t = decltype(&MoveTo_Impl);
    REL::Relocation<func_t> func{RE::Offset::TESObjectREFR::MoveTo};
    return func(ref, a_targetHandle, a_targetCell, a_selfWorldSpace, a_position, a_rotation);
}


void ObjectManipulationManager::SetPlacementState(ValidState id) {

    if (id != currentState) {
        currentState = id;

        for (auto [state, shader] : shaders) {
            if (state != id) {
                Papyrus::Stop(shader, pickObject);
            }
        }

        Papyrus::Play(shaders[id], pickObject);
    }
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
    auto validShaderId = dataHandler->LookupFormID(0x800, "ObjectManipulator.esp");
    auto validShader = RE::TESForm::LookupByID<RE::TESEffectShader>(validShaderId);
    auto invalidShaderId = dataHandler->LookupFormID(0x801, "ObjectManipulator.esp");
    auto invalidShader = RE::TESForm::LookupByID<RE::TESEffectShader>(invalidShaderId);

    shaders[ValidState::Valid] = validShader;
    shaders[ValidState::Error] = invalidShader;

    delete builder;
}




void ObjectManipulationManager::Pick(RE::TESObjectREFR* baseObject) {
    if (baseObject) {
        auto baseObj = baseObject->GetBaseObject();
        if (!baseObj) {
            return;
        }

        auto obj3d = baseObject->Get3D();
        if (obj3d) {
            auto currentLayer = obj3d->GetCollisionLayer();
            colisionLayer = currentLayer;
            if (currentLayer != RE::COL_LAYER::kNonCollidable) {
                obj3d->SetCollisionLayer(RE::COL_LAYER::kNonCollidable);
            }
        }

        lastPos = baseObject->GetPosition();
        lastAngle = baseObject->GetAngle();

        //placeholder->SetModel(modelBase->GetModel());
        //CreatePlaceholder();
        //if (!placeholderRef) {
        //    return;
        //}

        //auto scale = baseObject->GetScale();
        //Papyrus::SetScale(placeholderRef, scale);

        auto pos = baseObject->GetPosition();
        auto angle = baseObject->GetAngle().z;
        auto [cameraAngle, cameraPosition] = Utils::GetCameraData();
        angleOffset = normalizeAngle(angle - std::atan2(cameraPosition.x - pos.x, cameraPosition.y - pos.y));
        pickObject = baseObject;
        monitorState = MonitorState::Running;
        currentState = ValidState::None;
        
    }
}

void ObjectManipulationManager::Cancel() {
    monitorState = MonitorState::Idle;
    auto obj = pickObject;
    auto shader = shaders[currentState];
    currentState = ValidState::None;
    Papyrus::Stop(shader, obj);
    auto obj3d = pickObject->Get3D();
    auto layer = colisionLayer;
    if (obj3d) {
        obj3d->SetCollisionLayer(layer);
    }
    SKSE::GetTaskInterface()->AddTask([obj3d, layer, obj]() {
        MoveTo_Impl(obj, RE::ObjectRefHandle(), obj->GetParentCell(), obj->GetWorldspace(), lastPos, lastAngle);
        obj->Disable();
        obj->Enable(false);

    });

}
void ObjectManipulationManager::Release() {
        monitorState = MonitorState::Idle;
        auto obj = pickObject;
        auto shader = shaders[currentState];
        currentState = ValidState::None;
        Papyrus::Stop(shader, obj);

        auto obj3d = pickObject->Get3D();
        auto layer = colisionLayer;
        if (obj3d) {
            obj3d->SetCollisionLayer(layer);
        }
        if (!obj3d->AsBhkRigidBody()) {
            obj->SetPosition(obj3d->worldBound.center);
        }
}

void ObjectManipulationManager::UpdatePlaceholderPosition() {
    auto player = RE::PlayerCharacter::GetSingleton();
    if (pickObject->GetWorldspace() != player->GetWorldspace() ||
        pickObject->GetParentCell() != player->GetParentCell()) {
        MoveTo_Impl(pickObject, RE::ObjectRefHandle(), player->GetParentCell(),
                    player->GetWorldspace(),
                    player->GetPosition(), pickObject->GetAngle());
    }
}
void ObjectManipulationManager::Update() {
    auto obj = pickObject;

    auto state = &stateBuffer;
    SKSE::GetTaskInterface()->AddTask([obj, state]() {
        auto [cameraPosition, rayPostion] = Utils::PlayerCameraRayPos();
        UpdatePlaceholderPosition();
        Utils::SetPosition(obj, rayPostion);
        Utils::SetAngle(
            obj,
            RE::NiPoint3(
                0, 0, std::atan2(cameraPosition.x - rayPostion.x, cameraPosition.y - rayPostion.y) + angleOffset));
        obj->Update3DPosition(true);

        if (Utils::DistanceBetweenTwoPoints(cameraPosition, rayPostion) < 1000) {
            *state = ValidState::Valid;
        } else {
            *state = ValidState::Error;
        }
    });

    SetPlacementState(stateBuffer);

        //logger::info("Water: {}, name: {}", obj->IsInWater(),
        //                 RE::TES::GetSingleton()->GetLandTexture(rayPostion)->materialType->materialName);
}


void  ObjectManipulationManager::CameraHook::thunk(void* a1, RE::TESObjectREFR** refPtr) {
    originalFunction(a1, refPtr);
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
