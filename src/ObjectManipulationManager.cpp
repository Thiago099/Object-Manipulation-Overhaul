#include "ObjectManipulationManager.h"

void ObjectManipulationManager::ApplyShader(State id) {
    if (id != currentState) {
        SKSE::GetTaskInterface()->AddTask([id]() {
            shaderRef = pickedObject->ApplyEffectShader(shaders[id]);
        });
    }
}

void ObjectManipulationManager::CleanShader() {
    if (shaderRef) {
        shaderRef->ClearTarget();
        delete shaderRef;
    }
}

RE::TESEffectShader* LookUpShader(uint32_t id) {
    const auto dataHandler = RE::TESDataHandler::GetSingleton();
    auto shaderFormId = dataHandler->LookupFormID(id, "FX.esp");
    return RE::TESForm::LookupByID<RE::TESEffectShader>(shaderFormId);
}

void ObjectManipulationManager::Install() {
    auto hb = new HookBuilder();
    hb->AddCall<CameraHook, 5, 14>(49852, 0x50, 50784, 0x54);
    hb->Install();

    shaders[State::Valid] = LookUpShader(0x800);
    shaders[State::Error] = LookUpShader(0x801);
    shaders[State::Warn] = LookUpShader(0x802);

    delete hb;
}

void ObjectManipulationManager::Pick(RE::TESObjectREFR* obj) {
    if (obj) {
        pickedObject = obj;
        pickedObjectHasCollison = (obj->formFlags & RE::TESObjectREFR::RecordFlags::kCollisionsDisabled) == 0;
        if (pickedObjectHasCollison) {
            obj->formFlags |= RE::TESObjectREFR::RecordFlags::kCollisionsDisabled;
        }
    }

}

void ObjectManipulationManager::Release() {
    if (pickedObject) {
        if (pickedObjectHasCollison) {
            pickedObject->formFlags &= ~RE::TESObjectREFR::RecordFlags::kCollisionsDisabled;
        }
        pickedObject = nullptr;
        CleanShader();
    }

}

void  ObjectManipulationManager::CameraHook::thunk(void*, RE::TESObjectREFR** refPtr) {
        if (pickedObject) {

            auto obj = pickedObject;

            SKSE::GetTaskInterface()->AddTask([obj]() {

                RE::PlayerCamera* camera = RE::PlayerCamera::GetSingleton();
                auto thirdPerson =
                    reinterpret_cast<RE::ThirdPersonState*>(camera->cameraStates[RE::CameraState::kThirdPerson].get());
                auto firstPerson =
                    reinterpret_cast<RE::FirstPersonState*>(camera->cameraStates[RE::CameraState::kFirstPerson].get());
                auto player = RE::PlayerCharacter::GetSingleton();

                RE::NiQuaternion rotation;
                if (camera->currentState.get()->id == RE::CameraState::kFirstPerson) {
                    firstPerson->GetRotation(rotation);
                }
                if (camera->currentState.get()->id == RE::CameraState::kThirdPerson) {
                    rotation = thirdPerson->rotation;
                }

                auto pos = Utils::Raycast(player, rotation, camera->pos);

                Utils::SetPosition(obj, pos);
                Utils::SetAngle(obj,
                         RE::NiPoint3(0, 0, std::atan2(camera->pos.x - pos.x, camera->pos.y - pos.y) + M_PI));
                obj->Update3DPosition(true);

                if (Utils::DistanceBetweenTwoPoints(camera->pos, pos) < 1000) {
                    ApplyShader(State::Valid);
                } else {
                    ApplyShader(State::Error);
                }
            });
        }
}