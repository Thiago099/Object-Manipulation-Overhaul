// https://github.com/fenix31415/UselessFenixUtils/blob/afec377d09341bacc6abefa1b5105cabfd0bd249/src/Utils.cpp#L2017

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
template <int seid, int aeid, typename x_Function>
class skyrim_function;

template <int seid, int aeid, typename T, typename... Args>
class skyrim_function<seid,aeid, T(Args...)> {
public:
    static T eval(Args... args) {
        using func_t = T(Args...);
        REL::Relocation<func_t> func{REL::RelocationID(seid, aeid)};
        return func(std::forward<Args>(args)...);
    }
};
enum class LineOfSightLocation : uint32_t { kNone, kEyes, kHead, kTorso, kFeet };
// Function to convert quaternion to Euler angles (NiPoint3)
RE::NiPoint3 QuaternionToEuler(const RE::NiQuaternion& q) {
    RE::NiPoint3 euler;

    // Roll (x-axis rotation)
    double sinr_cosp = 2 * (q.w * q.x + q.y * q.z);
    double cosr_cosp = 1 - 2 * (q.x * q.x + q.y * q.y);
    euler.x = std::atan2(sinr_cosp, cosr_cosp);

    // Pitch (y-axis rotation)
    double sinp = 2 * (q.w * q.y - q.z * q.x);
    if (std::abs(sinp) >= 1)
        euler.y = std::copysign(M_PI / 2, sinp);  // use 90 degrees if out of range
    else
        euler.y =  std::asin(sinp);

    // Yaw (z-axis rotation)
    double siny_cosp = 2 * (q.w * q.z + q.x * q.y);
    double cosy_cosp = 1 - 2 * (q.y * q.y + q.z * q.z);
    euler.z =  std::atan2(siny_cosp, cosy_cosp);


    euler.x =  euler.x*-1;
    euler.y =  euler.y;
    euler.z =  euler.z*-1;

    return euler;
}
RE::NiPoint3 rotate(const RE::NiPoint3& A, const RE::NiPoint3& angles) {
    RE::NiMatrix3 R;
    R.EulerAnglesToAxesZXY(angles);
    return R * A;
}
RE::NiPoint3 rotate(float r, const RE::NiPoint3& angles) { return angles2dir(angles) * r; }
RE::NiPoint3 CalculateLOSLocation(RE::TESObjectREFR* refr, LineOfSightLocation los_loc) {
    return skyrim_function<46021, 47282, decltype(CalculateLOSLocation)>::eval(refr, los_loc);
}

RE::MagicTarget* FindPickTarget(RE::MagicCaster* caster, RE::NiPoint3& a_targetLocation, RE::TESObjectCELL** a_targetCell,
                                RE::bhkPickData& a_pickData) {
    using func_t = RE::MagicTarget* (RE::MagicCaster * caster, RE::NiPoint3 & a_targetLocation,
                                    RE::TESObjectCELL * *a_targetCell, RE::bhkPickData & a_pickData);
    REL::Relocation<func_t> func{RELOCATION_ID(33676, 34456)};
    return func(caster, a_targetLocation, a_targetCell, a_pickData);
}

RE::NiPoint3 raycast(RE::Actor* caster, RE::NiQuaternion angle, RE::NiPoint3 position) {
    auto havokWorldScale = RE::bhkWorld::GetWorldScale();
    RE::bhkPickData pick_data;
    RE::NiPoint3 ray_start, ray_end;

    //ray_start = CalculateLOSLocation(caster, LineOfSightLocation::kHead);
    ray_start = position;
    ray_end = ray_start + rotate(2000000000, QuaternionToEuler(angle));
    pick_data.rayInput.from = ray_start * havokWorldScale;
    pick_data.rayInput.to = ray_end * havokWorldScale;

    uint32_t collisionFilterInfo = 0;
    caster->GetCollisionFilterInfo(collisionFilterInfo);
    pick_data.rayInput.filterInfo = (static_cast<uint32_t>(collisionFilterInfo >> 16) << 16) |
                                    static_cast<uint32_t>(RE::COL_LAYER::kCharController);
    //RE::TES::GetSingleton()->Pick(pick_data);
    caster
        ->GetParentCell()
        ->GetbhkWorld()
        ->PickObject(pick_data);

    RE::NiPoint3 hitpos;
    if (pick_data.rayOutput.HasHit()) {
        hitpos = ray_start + (ray_end - ray_start) * pick_data.rayOutput.hitFraction;
    } else {
        hitpos = ray_end;
    }
    return hitpos;
}
