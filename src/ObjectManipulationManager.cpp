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



void ObjectManipulationManager::SetPlacementState(ValidState id) {
    if (id != currentState || currentState == ValidState::None) {
        currentState = id;
        auto obj = pickObject;
        auto color = stateColorMap[id];
        SKSE::GetTaskInterface()->AddTask([obj, color]() {
            if (auto obj3d = obj->Get3D()) {
                obj3d->TintScenegraph(color);
            } else {
                logger::info(":(");
            }
        });
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
    delete builder;
    stateColorMap[ValidState::Valid] = Utils::CreateColor(0x00CCFFCC);
    stateColorMap[ValidState::Error] = Utils::CreateColor(0xFF0000CC);
}

bool IsStatic(RE::COL_LAYER & col) {

    //logger::trace("col: {}", static_cast<int>(col));
    switch (col) {
        case RE::COL_LAYER::kUnidentified:
        case RE::COL_LAYER::kStatic: 
        case RE::COL_LAYER::kTrees:
        case RE::COL_LAYER::kAnimStatic:
        case RE::COL_LAYER::kTerrain:
        case RE::COL_LAYER::kTrap:
        case RE::COL_LAYER::kGround:
        case RE::COL_LAYER::kPortal:
            return true;
        default:
            return false;
    }
}

void ObjectManipulationManager::Pick(RE::TESObjectREFR* refr) {
    if (refr) {

        lastPos = refr->GetPosition();
        lastAngle = refr->GetAngle();
        auto [cameraAngle, cameraPosition] = Utils::GetCameraData();
        angleOffset =
            normalizeAngle(refr->GetAngle().z - std::atan2(cameraPosition.x - lastPos.x, cameraPosition.y - lastPos.y));
        positionOffset = RE::NiPoint3(0, 0, 0);
        ctrlKey = false;
        pickObject = refr;
        currentState = ValidState::None;
        stateBuffer = ValidState::Valid;
        monitorState = MonitorState::Running;

        auto obj3d = refr->Get3D();
        colisionLayer = obj3d->GetCollisionLayer();

        if (IsStatic(colisionLayer)) {
            obj3d->SetCollisionLayer(RE::COL_LAYER::kNonCollidable);
        }
    }
}

void ObjectManipulationManager::ResetCollision() {
    if (auto obj3d = pickObject->Get3D()) {
        SKSE::GetTaskInterface()->AddTask([obj3d]() { 
            auto color = RE::NiColorA(0, 0, 0, 0);
            obj3d->TintScenegraph(color); 
        });
        auto current = obj3d->GetCollisionLayer();
        if (current != colisionLayer) {
            obj3d->SetCollisionLayer(colisionLayer);
        }
    }
}

void ObjectManipulationManager::Cancel() {
    monitorState = MonitorState::Idle;
    auto obj = pickObject;
    auto obj3d = pickObject->Get3D();
    Utils::MoveTo_Impl(obj, RE::ObjectRefHandle(), obj->GetParentCell(), obj->GetWorldspace(), lastPos, lastAngle);
    ResetCollision();
}

void ObjectManipulationManager::Release() {
    monitorState = MonitorState::Idle;
    auto obj = pickObject;
    auto obj3d = pickObject->Get3D();
    if (IsStatic(colisionLayer)) {
        obj->SetPosition(obj->GetPosition());
    }
    ResetCollision();
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

    Utils::SetPosition(obj, rayPostion+positionOffset);
    Utils::SetAngle(
            obj,
            RE::NiPoint3(0, 0, std::atan2(cameraPosition.x - rayPostion.x, cameraPosition.y - rayPostion.y) + angleOffset));
    
    obj->Update3DPosition(true);

    if (Utils::DistanceBetweenTwoPoints(cameraPosition, rayPostion) < 1000) {
        SetPlacementState(ValidState::Valid);
    } else {
        SetPlacementState(ValidState::Error);
    }


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

                        auto player3d = Utils::GetPlayer3d();

                        const auto evaluator = [player3d](RE::NiAVObject* obj) {
                            if (obj == player3d) {
                                return false;
                            }
                            return true;
                        };
                        if (auto ref = Utils::PlayerCameraRayRefr(evaluator, 1000)){
                            Pick(ref);
                        }

                    }
                }

            }


        }

        originalFunction(a_dispatcher, a_event);
    }




}
