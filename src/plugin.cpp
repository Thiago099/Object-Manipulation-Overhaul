#include "Plugin.h"

#define M_PI 3.14159265358979323846

RE::TESObjectREFR* cheese;
void SetPosition(RE::TESObjectREFR* ref, const RE::NiPoint3& a_position) {
    if (!ref) {
        return;
    }
    using func_t = void(RE::TESObjectREFR*, const RE::NiPoint3&);
    REL::Relocation<func_t> func{RELOCATION_ID(19363, 19790)};
    return func(ref, a_position);
}
void SetAngle(RE::TESObjectREFR* ref, const RE::NiPoint3& a_position) {
    if (!ref) {
        return;
    }
    using func_t = void(RE::TESObjectREFR*, const RE::NiPoint3&);
    REL::Relocation<func_t> func{RELOCATION_ID(19359, 19786)};
    return func(ref, a_position);
}

void OnMessage(SKSE::MessagingInterface::Message* message) {
    if (message->type == SKSE::MessagingInterface::kDataLoaded) {
    }
    if (message->type == SKSE::MessagingInterface::kPostLoadGame || message->type == SKSE::MessagingInterface::kNewGame) {
        auto cheeseBase = RE::TESForm::LookupByID<RE::TESBoundObject>(0x15bab);
        auto cheeseRef = RE::PlayerCharacter::GetSingleton()->PlaceObjectAtMe(cheeseBase, true);
        cheese = cheeseRef.get();
        auto c = cheese;
        auto shader = RE::TESForm::LookupByID<RE::TESEffectShader>(0x11000800);
        SKSE::GetTaskInterface()->AddTask([c, shader]() {
            c->ApplyEffectShader(shader);
        });

    }
}

struct CameraHook {
    static void thunk(void* prop, RE::TESObjectREFR** refPtr) {
        if (cheese) {
            RE::PlayerCamera* camera = RE::PlayerCamera::GetSingleton();
            auto thirdPerson = reinterpret_cast<RE::ThirdPersonState*>(camera->cameraStates[RE::CameraState::kThirdPerson].get());
            auto firstPerson = reinterpret_cast<RE::FirstPersonState*>(camera->cameraStates[RE::CameraState::kFirstPerson].get());
            auto player = RE::PlayerCharacter::GetSingleton();
  
            RE::NiQuaternion rotation;
            if (camera->currentState.get()->id == RE::CameraState::kFirstPerson) {
                firstPerson->GetRotation(rotation);
            }
            if (camera->currentState.get()->id == RE::CameraState::kThirdPerson) {
                rotation = thirdPerson->rotation;
            }

            auto pos = raycast(player, rotation, camera->pos);
  
            auto c = cheese;
            c->formFlags |= RE::TESObjectREFR::RecordFlags::kCollisionsDisabled;
            SKSE::GetTaskInterface()->AddTask([c, player, pos, camera]() {
                SetPosition(c, pos);
                SetAngle(c,
                         RE::NiPoint3(0, 0,
                                      std::atan2(camera->pos.x - pos.x,
                                                 camera->pos.y - pos.y)+M_PI));
                cheese->Update3DPosition(true);
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