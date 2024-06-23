namespace Utils {
    std::pair<RE::NiPoint3, RE::NiPoint3> PlayerCameraRay();
    RE::NiPoint3 Raycast(RE::Actor* caster, RE::NiQuaternion angle, RE::NiPoint3 position);
    RE::TESObjectREFR* PickObject();
    void SetPosition(RE::TESObjectREFR* ref, const RE::NiPoint3& a_position);
    void SetAngle(RE::TESObjectREFR* ref, const RE::NiPoint3& a_position);

    void StopVisualEffect(RE::TESObjectREFR* r, void* e);
    float DistanceBetweenTwoPoints(RE::NiPoint3& a, RE::NiPoint3& b);
    void CallPapyrusAction(RE::TESObjectREFR* obj, const char* className, const char* methodName);
}