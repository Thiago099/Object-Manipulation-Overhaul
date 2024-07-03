#include "Application/ObjectManipulationManager.h"
 void ObjectManipulationManager::Install() {
    auto builder = new HookBuilder();
    
    builder->AddCall<Input::ProcessInputQueueHook, 5, 14>(67315, 0x7B, 68617, 0x7B, 0xC519E0, 0x81);
    builder->Install();
    
    delete builder;

    State::stateColorMap[State::ValidState::Valid] = Misc::CreateColor(0x00CCFFaa);
    State::stateColorMap[State::ValidState::Error] = Misc::CreateColor(0xFF0000aa);

    Input::passiveInputManager->AddSink("Pick", Input::PassiveState::Pick);
    Input::activeInputManager->AddSink("Cancel", Input::ActiveState::Cancel);
    Input::activeInputManager->AddSink("Commit", Input::ActiveState::Commit);

    Input::activeInputManager->AddSink("ToggleRotate", Input::ActiveState::ToggleRotate);
    Input::activeInputManager->AddSink("ToggleMove", Input::ActiveState::ToggleMove);
    Input::activeInputManager->AddSink("ResetTransform", Input::ActiveState::ResetTransform);
    Input::activeInputManager->AddSink("AdvancedMode", Input::ActiveState::AdvancedMode);
 }

void ObjectManipulationManager::StartDraggingObject(RE::TESObjectREFR* ref) {
    if (ref) {
        CancelDrag();
        Selection::lastPosition = ref->GetPosition();
        Selection::lastAngle = ref->GetAngle();
        auto [cameraAngle, cameraPosition] = RayCast::GetCameraData();
        auto angle = ref->GetAngle();
        Selection::rotateOffset = glm::vec2(-angle.z + cameraAngle.z, 0);
        Selection::moveOffset = glm::vec2(0, 0);
        Input::isToggleMoveDown = false;
        Input::isToggleRotateDown = false;
        Input::IsAdvancedMode = false;
        Selection::object = ref;
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
    } else {
        SKSE::GetTaskInterface()->AddTask([obj]() {
            if (obj) {
                obj->Update3DPosition(true);
            }
        });
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

    const auto evaluator = [player3d, pick3d,obj](RE::NiAVObject* mesh) {
        if (mesh == player3d) {
            return false;
        }
        if (mesh->GetUserData() == obj) {
            return false;
        }

        return true;
    };

    auto ray = RayCast::Cast(evaluator);

    auto [cameraAngle, cameraPosition] = RayCast::GetCameraData();

    Selection::UpdateObjectTransform(ray.position);

    if (Misc::DistanceBetweenTwoPoints(cameraPosition, ray.position) < 1000) {
        SetPlacementState(State::ValidState::Valid);
    } else {
        SetPlacementState(State::ValidState::Error);
    }

    if (ray.object) {
    } else {

        if (auto texture = RE::TES::GetSingleton()->GetLandTexture(ray.position)) {
            if (auto material = texture->materialType) {
                logger::trace("ID: {}, Name: {}", material->materialID, material->materialName);
            }
            logger::info("IsInWater: {}",obj->IsInWater());
        }

    }

}

InputManager* ObjectManipulationManager::GetPassiveInputManager() { return Input::passiveInputManager; }

InputManager* ObjectManipulationManager::GetActiveInputManager() { return Input::activeInputManager; }

ObjectReferenceFilter* ObjectManipulationManager::GetRaycastReferenceFilter() { return Selection::objectReferneceFilter; }

void ObjectManipulationManager::SetdoToggleWithToggleKey(bool value) {
    Input::doToggleWithToggleKey = value;
}

void ObjectManipulationManager::Clean() {
    State::dragState = State::DragState::Idle;
}
RE::NiPoint3 glmPointToSkyrim(glm::vec3 point) {
    return {point.x,point.y, point.z};
}
inline void ObjectManipulationManager::Selection::UpdateObjectTransform(RE::NiPoint3& rayPosition) {
    auto obj = Selection::object;
    auto [cameraAngle, cameraPosition] = RayCast::GetCameraData();

    auto yoffset = Selection::rotateOffset.y;
    auto xoffset = Selection::rotateOffset.x;


    auto c = glm::rotate(glm::mat4(1.0f), -cameraAngle.z, glm::vec3(1.0f, 0.0f, 0.0f));
    auto b = glm::rotate(glm::mat4(1.0f), yoffset, glm::vec3(0.0f, 0.0f, 1.0f));
    auto a = glm::rotate(glm::mat4(1.0f), xoffset, glm::vec3(1.0f, 0.0f, 0.0f));

    auto rotationMatrix = a * b * c;

    float newYaw = atan2(rotationMatrix[1][0], rotationMatrix[0][0]);
    float newPitch = asin(-rotationMatrix[2][0]);
    float newRoll = atan2(rotationMatrix[2][1], rotationMatrix[2][2]);

    float x = Selection::moveOffset.x * cos(-cameraAngle.z);
    float y = Selection::moveOffset.x * sin(-cameraAngle.z);
    float z = -Selection::moveOffset.y;

    Misc::SetPosition(obj, rayPosition + RE::NiPoint3(x, y, z));
    Misc::SetAngle(obj, RE::NiPoint3(newYaw, newPitch, newRoll));

    obj->Update3DPosition(true);
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
        if(Misc::DoesButtonTriggerAction(button, "Activate")){
            return true;
        }
    }
    return false;
}

glm::vec3 extractEulerAngles(const glm::mat3& R) {
    float pitch = asin(-R[2][0]);
    float yaw = atan2(R[1][0], R[0][0]);
    float roll = atan2(R[2][1], R[2][2]);

    return glm::vec3(yaw, pitch, roll);
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
            if (auto move = current->AsMouseMoveEvent() ) {
                if (ActiveState::ProcessMouseMovement(move)) {
                    suppress = true;
                }
            }

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

bool ObjectManipulationManager::Input::ActiveState::ProcessMouseMovement(RE::MouseMoveEvent* move) {
    if (Input::isToggleRotateDown) {
        auto sensitivity = 0.005;
        if (Input::IsAdvancedMode) {
            Selection::rotateOffset.y += move->mouseInputY * sensitivity;
        }

        Selection::rotateOffset.y = Misc::NormalizeAngle(Selection::rotateOffset.y);

        if (glm::abs(Selection::rotateOffset.y) > glm::half_pi<float>()) {
            Selection::rotateOffset.x -= move->mouseInputX * sensitivity;
        } else {
            Selection::rotateOffset.x += move->mouseInputX * sensitivity;
        }
        Selection::rotateOffset.y = Misc::NormalizeAngle(Selection::rotateOffset.y);
        return true;
    } else if (Input::isToggleMoveDown) {
        auto sensitivity = 0.1;
        if (Input::IsAdvancedMode) {
            Selection::moveOffset.x += move->mouseInputX * sensitivity;
        }
        Selection::moveOffset.y += move->mouseInputY * sensitivity;
        return true;
    }
    return false;
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
        auto ray = RayCast::Cast(evaluator, 1000);
        if (ray.object) {
            if (Selection::objectReferneceFilter->Match(ray.object)) {
                StartDraggingObject(ray.object);
            }
        }
    }
}


void ObjectManipulationManager::Input::ActiveState::ToggleMove(RE::ButtonEvent* button) {
    if (button->IsDown()) {
        Input::isToggleMoveDown = true;
    } else if (button->IsUp()) {
        Input::isToggleMoveDown = false;
    }
}
void ObjectManipulationManager::Input::ActiveState::ToggleRotate(RE::ButtonEvent* button) {
    if (button->IsDown()) {
        Input::isToggleRotateDown = true;
    } else if (button->IsUp()) {
        Input::isToggleRotateDown = false;
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
void ObjectManipulationManager::Input::ActiveState::ResetTransform(RE::ButtonEvent* button) {
    if (button->IsDown()) {
        if (Input::isToggleMoveDown) {
            Selection::moveOffset = glm::vec2(0, 0);
        }
        else if (Input::isToggleRotateDown) {
            Selection::rotateOffset = glm::vec2(0, 0);
        } else
        {
            Selection::moveOffset = glm::vec2(0, 0);
            Selection::rotateOffset = glm::vec2(0, 0);
        }
    }
}

void ObjectManipulationManager::Input::ActiveState::AdvancedMode(RE::ButtonEvent* button) {
    if (button->IsDown()) {
        Input::IsAdvancedMode = !IsAdvancedMode;
    } 
}




