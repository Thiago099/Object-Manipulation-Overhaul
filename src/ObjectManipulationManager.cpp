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

    if (id != currentState || currentState == ValidState::None) {
        currentState = id;

        for (auto [state, shader] : shaders) {
            if (state != id) {
                Papyrus::Stop(shader, pickObject);
            }
        }
        logger::trace("shader id: {}", id);
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




void ObjectManipulationManager::Pick(RE::TESObjectREFR* refr) {
    if (refr) {

        auto obj3d = refr->Get3D();
        if (obj3d) {
            auto currentLayer = obj3d->GetCollisionLayer();
            colisionLayer = currentLayer;
            if (currentLayer != RE::COL_LAYER::kNonCollidable) {
                obj3d->SetCollisionLayer(RE::COL_LAYER::kNonCollidable);
            }
        }

        lastPos = refr->GetPosition();
        lastAngle = refr->GetAngle();

        //placeholder->SetModel(modelBase->GetModel());
        //CreatePlaceholder();
        //if (!placeholderRef) {
        //    return;
        //}

        //auto scale = baseObject->GetScale();
        //Papyrus::SetScale(placeholderRef, scale);

        auto pos = refr->GetPosition();
        auto angle = refr->GetAngle().z;
        auto [cameraAngle, cameraPosition] = Utils::GetCameraData();
        angleOffset = normalizeAngle(angle - std::atan2(cameraPosition.x - pos.x, cameraPosition.y - pos.y));
        positionOffset = RE::NiPoint3(0, 0, 0);
        ctrlKey = false;
        pickObject = refr;
        currentState = ValidState::None;
        stateBuffer = ValidState::Valid;
        monitorState = MonitorState::Running;
    }
}

void ObjectManipulationManager::Cancel() {
    monitorState = MonitorState::Idle;
    auto obj = pickObject;
    auto shader = shaders[currentState];
    Papyrus::Stop(shader, obj);
    auto obj3d = pickObject->Get3D();
    auto layer = colisionLayer;
    if (obj3d) {
        obj3d->SetCollisionLayer(layer);
    }
    SKSE::GetTaskInterface()->AddTask([obj3d, layer, obj]() {
        MoveTo_Impl(obj, RE::ObjectRefHandle(), obj->GetParentCell(), obj->GetWorldspace(), lastPos, lastAngle);
    });

}
void ObjectManipulationManager::Release() {
        monitorState = MonitorState::Idle;
        auto obj = pickObject;
        auto shader = shaders[currentState];
        SKSE::GetTaskInterface()->AddTask([shader, obj]() { Papyrus::Stop(shader, obj); });

        auto obj3d = pickObject->Get3D();
        auto layer = colisionLayer;
        if (obj3d) {
            obj3d->SetCollisionLayer(layer);
        }
        if (!obj3d->AsBhkRigidBody() && obj->GetFormType() != RE::FormType::NPC&& obj->GetFormType() != RE::FormType::ActorCharacter) {
            SKSE::GetTaskInterface()->AddTask([obj]() {
                obj->Disable();
                obj->Enable(false);
            });
        }
}

void ObjectManipulationManager::UpdatePlaceholderPosition() {
    auto player = RE::PlayerCharacter::GetSingleton();
    if (pickObject->GetWorldspace() != player->GetWorldspace() ||
        pickObject->GetParentCell() != player->GetParentCell()) {
        MoveTo_Impl(pickObject, RE::ObjectRefHandle(), player->GetParentCell(),
                    player->GetWorldspace(),
                    player->GetPosition(), pickObject->GetAngle());
        currentState = ValidState::None;
        auto obj3d = pickObject->Get3D();
        if (obj3d) {
            auto currentLayer = obj3d->GetCollisionLayer();
            if (currentLayer != RE::COL_LAYER::kNonCollidable) {
                obj3d->SetCollisionLayer(RE::COL_LAYER::kNonCollidable);
            }
        }
    }
}
void ObjectManipulationManager::Update() {
    auto obj = pickObject;

    auto state = &stateBuffer;
    auto [cameraPosition, rayPostion] = Utils::PlayerCameraRayPos();
    UpdatePlaceholderPosition();
    Utils::SetPosition(obj, rayPostion+positionOffset);
        
    Utils::SetAngle(
            obj,
            RE::NiPoint3(obj->GetAngleX(), obj->GetAngleY(),
                         std::atan2(cameraPosition.x - rayPostion.x, cameraPosition.y - rayPostion.y) + angleOffset));
    obj->Update3DPosition(true);

    if (Utils::DistanceBetweenTwoPoints(cameraPosition, rayPostion) < 1000) {
        *state = ValidState::Valid;
    } else {
        *state = ValidState::Error;
    }

    SetPlacementState(stateBuffer);

        //logger::info("Water: {}, name: {}", obj->IsInWater(),
        //                 RE::TES::GetSingleton()->GetLandTexture(rayPostion)->materialType->materialName);
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

                if (button->GetDevice() == RE::INPUT_DEVICE::kKeyboard) {
                        switch (auto key = static_cast<RE::BSKeyboardDevice::Key>(button->GetIDCode())) {
                        case RE::BSKeyboardDevice::Key::kLeftControl:
                        case RE::BSKeyboardDevice::Key::kRightControl:
                            if (button->IsDown()) {
                                ctrlKey = true;
                            } else if (button->IsUp()) {
                                ctrlKey = false;
                            }
                            suppress = true;
                            break;
                            default:
                                break;
                        }
                }
                if (button->GetDevice() == RE::INPUT_DEVICE::kMouse) {


                    switch (auto key = static_cast<RE::BSWin32MouseDevice::Key>(button->GetIDCode())) {
                        case RE::BSWin32MouseDevice::Key::kWheelUp:
                            suppress = true;
                            if (ctrlKey) {
                                positionOffset.z += 1.f;
                            } else {
                                angleOffset = normalizeAngle(angleOffset + M_PI / 30);
                            }
                            break;
                        case RE::BSWin32MouseDevice::Key::kWheelDown:
                            suppress = true;
                            if (ctrlKey) {
                                positionOffset.z -= 1.f;
                            } else {
                                angleOffset = normalizeAngle(angleOffset - M_PI / 30);
                            }
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
                        
                        if (auto ref = Utils::PlayerCameraRayRefr()) {
                            Pick(ref);
                        }

                    }
                }

            }


        }

        originalFunction(a_dispatcher, a_event);
    }




}
