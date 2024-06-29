#include "Lib/Misc.h"

void Misc::SetPosition(RE::TESObjectREFR* ref, const RE::NiPoint3& a_position) {
    if (!ref) {
        return;
    }
    using func_t = void(RE::TESObjectREFR*, const RE::NiPoint3&);
    REL::Relocation<func_t> func{RELOCATION_ID(19363, 19790)};
    return func(ref, a_position);
}

void Misc::SetAngle(RE::TESObjectREFR* ref, const RE::NiPoint3& a_position) {
    if (!ref) {
        return;
    }
    using func_t = void(RE::TESObjectREFR*, const RE::NiPoint3&);
    REL::Relocation<func_t> func{RELOCATION_ID(19359, 19786)};
    return func(ref, a_position);
}

float Misc::DistanceBetweenTwoPoints(RE::NiPoint3& a, RE::NiPoint3& b) {
    double dx = b.x - a.x;
    double dy = b.y - a.y;
    double dz = b.z - a.z;
    return std::sqrt(dx * dx + dy * dy + dz * dz);
}

double Misc::NormalizeAngle(double angle_rad) {
    // Normalize angle to be within [0, 2*pi)
    while (angle_rad < 0) {
        angle_rad += 2 * M_PI;
    }
    while (angle_rad >= 2 * M_PI) {
        angle_rad -= 2 * M_PI;
    }
    return angle_rad;
}

RE::NiColorA Misc::CreateColor(uint32_t color) {
    // Extracting RGB components from the hex value
    float red = ((color & 0xFF000000) >> 24) / 255.0f;
    float green = ((color & 0x00FF0000) >> 16) / 255.0f;
    float blue = ((color & 0x0000FF00) >> 8) / 255.0f;
    float alpha = (color & 0x000000FF) / 255.0f;

    return RE::NiColorA(red, green, blue, alpha);
}

bool Misc::IsStatic(RE::COL_LAYER& col) {
    // logger::trace("col: {}", static_cast<int>(col));
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

RE::NiObject* Misc::GetPlayer3d() {
    auto refr = RE::PlayerCharacter::GetSingleton();
    if (!refr) {
        return nullptr;
    }
    if (!refr->loadedData) {
        return nullptr;
    }
    if (!refr->loadedData->data3D) {
        return nullptr;
    }
    return refr->loadedData->data3D.get();
}

void Misc::MoveTo_Impl(RE::TESObjectREFR* ref, const RE::ObjectRefHandle& a_targetHandle,
                        RE::TESObjectCELL* a_targetCell, RE::TESWorldSpace* a_selfWorldSpace,
                        const RE::NiPoint3& a_position, const RE::NiPoint3& a_rotation) {
    using func_t = decltype(&MoveTo_Impl);
    REL::Relocation<func_t> func{RE::Offset::TESObjectREFR::MoveTo};
    return func(ref, a_targetHandle, a_targetCell, a_selfWorldSpace, a_position, a_rotation);
}










