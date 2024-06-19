#include "ObjectManipulationManager.h"

void ObjectManipulationManager::Install() {
    auto hb = new HookBuilder();
    hb->AddCall<CameraHook, 5, 14>(49852, 0x50, 50784, 0x54);
    hb->Install();
    delete hb;
}

void ObjectManipulationManager::Pick(RE::TESObjectREFR* obj) {
    pickedObject = obj;
    SKSE::GetTaskInterface()->AddTask([obj]() {
        const auto dataHandler = RE::TESDataHandler::GetSingleton();
        auto shaderFormId = dataHandler->LookupFormID(0x800, "FX.esp");
        auto shader = RE::TESForm::LookupByID<RE::TESEffectShader>(shaderFormId);
        shaderRef = obj->ApplyEffectShader(shader);
    });
}

void ObjectManipulationManager::Release(RE::TESObjectREFR* obj) {
    pickedObject = nullptr;
    if (shaderRef) {
        shaderRef->ClearTarget();
        delete shaderRef;
    }
}

void  ObjectManipulationManager::CameraHook::thunk(void*, RE::TESObjectREFR** refPtr) {
        if (pickedObject) {

            auto obj = pickedObject;

            //pickedObject->formFlags |= RE::TESObjectREFR::RecordFlags::kCollisionsDisabled;
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
            });
        }
}