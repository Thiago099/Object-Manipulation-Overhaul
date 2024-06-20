namespace Utils {
    RE::NiPoint3 Raycast(RE::Actor* caster, RE::NiQuaternion angle, RE::NiPoint3 position);
    void SetPosition(RE::TESObjectREFR* ref, const RE::NiPoint3& a_position);
    void SetAngle(RE::TESObjectREFR* ref, const RE::NiPoint3& a_position);
    void StopVisualEffect(RE::TESEffectShader* e, RE::TESObjectREFR* r);
    float DistanceBetweenTwoPoints(RE::NiPoint3& a, RE::NiPoint3& b);
}