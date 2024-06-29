#pragma once
#define M_PI 3.14159265358979323846

namespace Misc {
    void SetPosition(RE::TESObjectREFR* ref, const RE::NiPoint3& a_position);
    void SetAngle(RE::TESObjectREFR* ref, const RE::NiPoint3& a_position);
    float DistanceBetweenTwoPoints(RE::NiPoint3& a, RE::NiPoint3& b);
    double NormalizeAngle(double angle_rad);
    RE::NiColorA CreateColor(uint32_t color);
    bool IsStatic(RE::COL_LAYER& col);
    RE::NiObject* GetPlayer3d();
    void MoveTo_Impl(RE::TESObjectREFR* ref, const RE::ObjectRefHandle& a_targetHandle, RE::TESObjectCELL* a_targetCell,
                     RE::TESWorldSpace* a_selfWorldSpace, const RE::NiPoint3& a_position,
                     const RE::NiPoint3& a_rotation);
    RE::FormType StringToFormType(const std::string& str);
    std::string ToLowerCase(const std::string& str);
}

