#pragma once
#include "Lib/RayCollector.h"
struct RayCastResult;
class RayCast {
        static std::pair<RE::NiPoint3, RE::TESObjectREFR*> CastRay(
        RE::Actor* caster, RE::NiPoint3 angle,
                                                                  RE::NiPoint3 position,
                                                                  std::function<bool(RE::NiAVObject*)> const& evaluator,
                                                                  float raySize);
    public:
        static RE::NiPoint3 QuaternionToEuler(const RE::NiQuaternion& q);
        static std::pair<RE::NiPoint3, RE::NiPoint3> GetCameraData();
        static RayCastResult Cast(std::function<bool(RE::NiAVObject*)> const& evaluator, float raySize = 2000000000.f);


};

struct RayCastResult {
    RE::NiPoint3 position;
    RE::TESObjectREFR* object;
};