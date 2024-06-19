#include "Plugin.h"

RE::TESObjectREFR* cheese;

void MoveObjectTo(
        RE::TESObjectREFR* ref,
        const RE::ObjectRefHandle& handle,
        RE::TESObjectCELL* a_targetCell,
        RE::TESWorldSpace* a_selfWorldSpace,
        const RE::NiPoint3& a_position,
        const RE::NiPoint3& a_rotation
    ) {
    if (!ref) {
        return;
    }
    using func_t = void(RE::TESObjectREFR*,const RE::ObjectRefHandle&, RE::TESObjectCELL*, RE::TESWorldSpace*, const RE::NiPoint3&, const RE::NiPoint3&);
        REL::Relocation<func_t> func{RE::Offset::TESObjectREFR::MoveTo};
    return func(ref, handle, a_targetCell, a_selfWorldSpace, a_position, a_rotation);
}

void OnMessage(SKSE::MessagingInterface::Message* message) {
    if (message->type == SKSE::MessagingInterface::kDataLoaded) {
    }
    if (message->type == SKSE::MessagingInterface::kPostLoadGame || message->type == SKSE::MessagingInterface::kNewGame) {
        auto cheeseBase = RE::TESForm::LookupByID<RE::TESBoundObject>(0x64B33);
        auto cheeseRef = RE::PlayerCharacter::GetSingleton()->PlaceObjectAtMe(cheeseBase, true);
        cheese = cheeseRef.get();
    }
}

struct CameraHook {
    static void thunk(void* prop, RE::TESObjectREFR** refPtr) {
        if (cheese) {
            RE::PlayerCamera* camera = RE::PlayerCamera::GetSingleton();
            auto thirdPerson = reinterpret_cast<RE::ThirdPersonState*>(camera->cameraStates[RE::CameraState::kThirdPerson].get());
            auto firstPerson = reinterpret_cast<RE::FirstPersonState*>(camera->cameraStates[RE::CameraState::kFirstPerson].get());
            auto player = RE::PlayerCharacter::GetSingleton();
  
                //logger::info("pos: x: {},y: {},x:{}", thirdPerson->rotation.x, thirdPerson->rotation.y,
                //         thirdPerson->rotation.z);

            RE::NiQuaternion rotation;
            if (camera->currentState.get()->id == RE::CameraState::kFirstPerson) {
                firstPerson->GetRotation(rotation);
            }
            if (camera->currentState.get()->id == RE::CameraState::kThirdPerson) {
                rotation = thirdPerson->rotation;
            }

            auto pos = raycast(player, rotation, camera->pos);
  
            auto c = cheese;
            logger::info("pos: x: {},y: {},x:{}", pos.x, pos.y, pos.z);
            SKSE::GetTaskInterface()->AddTask([c, player, pos]() {
                MoveObjectTo(c, c->GetHandle(), player->GetParentCell(), player->GetWorldspace(), pos,
                            RE::NiPoint3(0, 0, 0));
            });

        }
    }
    static inline REL::Relocation<decltype(thunk)> originalFunction;
};
SKSEPluginLoad(const SKSE::LoadInterface *skse) {
    SKSE::Init(skse);
    SKSE::GetMessagingInterface()->RegisterListener(OnMessage);
    SetupLog();
    logger::info("Plugin loaded");
    UI::Register();

    auto hb = new HookBuilder();
    logger::info("init");   

    hb->AddCall<CameraHook, 5, 14>(49852, 0x50, 50784, 0x54);
    hb->Install();
    delete hb;

    return true;
}