#pragma once
#define M_PI 3.14159265358979323846
#include "Lib/RayCollector.h"
class RayCast {
    public:
    static std::pair<RE::NiQuaternion, RE::NiPoint3> GetCameraData();
    static std::pair<RE::NiPoint3, RE::NiPoint3> GetCursorPosition(std::function<bool(RE::NiAVObject*)> const& evaluator);
    static RE::TESObjectREFR* GetObjectAtCursor(std::function<bool(RE::NiAVObject*)> const& evaluator, float raySize);
    static std::pair<RE::NiPoint3, RE::TESObjectREFR*> RaycastObjectRefr(
        RE::Actor* caster, RE::NiQuaternion angle,
                                                                  RE::NiPoint3 position,
                                                                  std::function<bool(RE::NiAVObject*)> const& evaluator,
                                                                  float raySize);
};