#include "Utils.h"

#define M_PI 3.14159265358979323846

RE::NiPoint3 angles2dir(const RE::NiPoint3& angles) {
    RE::NiPoint3 ans;

    float sinx = sinf(angles.x);
    float cosx = cosf(angles.x);
    float sinz = sinf(angles.z);
    float cosz = cosf(angles.z);

    ans.x = cosx * sinz;
    ans.y = cosx * cosz;
    ans.z = -sinx;

    return ans;
}
enum class LineOfSightLocation : uint32_t { kNone, kEyes, kHead, kTorso, kFeet };
RE::NiPoint3 QuaternionToEuler(const RE::NiQuaternion& q) {
    RE::NiPoint3 euler;

    double sinr_cosp = 2 * (q.w * q.x + q.y * q.z);
    double cosr_cosp = 1 - 2 * (q.x * q.x + q.y * q.y);
    euler.x = std::atan2(sinr_cosp, cosr_cosp);

    // Pitch (y-axis rotation)
    double sinp = 2 * (q.w * q.y - q.z * q.x);
    if (std::abs(sinp) >= 1)
        euler.y = std::copysign(M_PI / 2, sinp);
    else
        euler.y = std::asin(sinp);

    // Yaw (z-axis rotation)
    double siny_cosp = 2 * (q.w * q.z + q.x * q.y);
    double cosy_cosp = 1 - 2 * (q.y * q.y + q.z * q.z);
    euler.z = std::atan2(siny_cosp, cosy_cosp);

    euler.x = euler.x * -1;
    euler.y = euler.y;
    euler.z = euler.z * -1;

    return euler;
}
RE::NiPoint3 rotate(const RE::NiPoint3& A, const RE::NiPoint3& angles) {
    RE::NiMatrix3 R;
    R.EulerAnglesToAxesZXY(angles);
    return R * A;
}
RE::NiPoint3 rotate(float r, const RE::NiPoint3& angles) { return angles2dir(angles) * r; }

RE::MagicTarget* FindPickTarget(RE::MagicCaster* caster, RE::NiPoint3& a_targetLocation,
                                RE::TESObjectCELL** a_targetCell, RE::bhkPickData& a_pickData) {
    using func_t = RE::MagicTarget*(RE::MagicCaster * caster, RE::NiPoint3 & a_targetLocation,
                                    RE::TESObjectCELL * *a_targetCell, RE::bhkPickData & a_pickData);
    REL::Relocation<func_t> func{RELOCATION_ID(33676, 34456)};
    return func(caster, a_targetLocation, a_targetCell, a_pickData);
}

RE::NiPoint3 Utils::Raycast(RE::Actor* caster, RE::NiQuaternion angle, RE::NiPoint3 position) {
    auto havokWorldScale = RE::bhkWorld::GetWorldScale();
    RE::bhkPickData pick_data;
    RE::NiPoint3 ray_start, ray_end;

    ray_start = position;
    ray_end = ray_start + rotate(2000000000, QuaternionToEuler(angle));
    pick_data.rayInput.from = ray_start * havokWorldScale;
    pick_data.rayInput.to = ray_end * havokWorldScale;

    uint32_t collisionFilterInfo = 0;
    caster->GetCollisionFilterInfo(collisionFilterInfo);
    pick_data.rayInput.filterInfo = (static_cast<uint32_t>(collisionFilterInfo >> 16) << 16) |
                                    static_cast<uint32_t>(RE::COL_LAYER::kCharController);

    caster->GetParentCell()->GetbhkWorld()->PickObject(pick_data);

    RE::NiPoint3 hitpos;
    if (pick_data.rayOutput.HasHit()) {
        hitpos = ray_start + (ray_end - ray_start) * pick_data.rayOutput.hitFraction;
    } else {
        hitpos = ray_end;
    }
    return hitpos;
}

void Utils::SetPosition(RE::TESObjectREFR* ref, const RE::NiPoint3& a_position) {
    if (!ref) {
        return;
    }
    using func_t = void(RE::TESObjectREFR*, const RE::NiPoint3&);
    REL::Relocation<func_t> func{RELOCATION_ID(19363, 19790)};
    return func(ref, a_position);
}

void Utils::SetAngle(RE::TESObjectREFR* ref, const RE::NiPoint3& a_position) {
    if (!ref) {
        return;
    }
    using func_t = void(RE::TESObjectREFR*, const RE::NiPoint3&);
    REL::Relocation<func_t> func{RELOCATION_ID(19359, 19786)};
    return func(ref, a_position);
}




REL::Relocation<void*> singleton{RELOCATION_ID(514167, 400315)};

void Utils::StopVisualEffect(RE::TESObjectREFR* r, void* ptr) {
    if (r == nullptr) {
        return;
    }
    using func_t = void(void*,RE::TESObjectREFR*,void*);
    REL::Relocation<func_t> func{RELOCATION_ID(40381, 41395)};
    return func(singleton.get(), r, ptr);
}



float Utils::DistanceBetweenTwoPoints(RE::NiPoint3& a, RE::NiPoint3& b){
    double dx = b.x - a.x;
    double dy = b.y - a.y;
    double dz = b.z - a.z;
    return std::sqrt(dx * dx + dy * dy + dz * dz);
}

void Utils::CallPapyrusAction(RE::TESObjectREFR* obj, const char* className, const char* methodName) {
    auto* vm = RE::BSScript::Internal::VirtualMachine::GetSingleton();
    auto handlePolicy = vm->GetObjectHandlePolicy();
    auto args = RE::MakeFunctionArguments();
    RE::VMHandle handle = handlePolicy->GetHandleForObject(RE::FormType::Reference, obj);
    auto callback = RE::BSTSmartPointer<RE::BSScript::IStackCallbackFunctor>();
    bool success =
        vm->DispatchMethodCall(handle, RE::BSFixedString(className), RE::BSFixedString(methodName),
                                          args, callback);
    handlePolicy->ReleaseHandle(handle);
}