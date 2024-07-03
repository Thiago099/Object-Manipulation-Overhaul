#include "Lib/Raycast.h"

float pointDistance(RE::NiPoint3 a, RE::NiPoint3 b) {
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    float dz = a.z - b.z;
    return std::sqrt(dx * dx + dy * dy + dz * dz);
}

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
RE::NiPoint3 RayCast::QuaternionToEuler(const RE::NiQuaternion& q) {
    RE::NiPoint3 euler;

    double sinr_cosp = 2 * (q.w * q.x + q.y * q.z);
    double cosr_cosp = 1 - 2 * (q.x * q.x + q.y * q.y);
    euler.x = std::atan2(sinr_cosp, cosr_cosp);

    // Pitch (y-axis rotation)
    double sinp = 2 * (q.w * q.y - q.z * q.x);
    if (std::abs(sinp) >= 1)
        euler.y = std::copysign(glm::pi<float>() / 2, sinp);
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

std::pair<RE::NiPoint3, RE::NiPoint3> RayCast::GetCameraData() {
    RE::PlayerCamera* camera = RE::PlayerCamera::GetSingleton();
    auto thirdPerson =
        reinterpret_cast<RE::ThirdPersonState*>(camera->cameraStates[RE::CameraState::kThirdPerson].get());
    auto firstPerson =
        reinterpret_cast<RE::FirstPersonState*>(camera->cameraStates[RE::CameraState::kFirstPerson].get());

    RE::NiQuaternion rotation;
    RE::NiPoint3 translation;
    if (camera->currentState.get()->id == RE::CameraState::kFirstPerson) {
        firstPerson->GetRotation(rotation);
        firstPerson->GetTranslation(translation);
        translation += firstPerson->dampeningOffset;
    } else if (camera->currentState.get()->id == RE::CameraState::kThirdPerson) {
        rotation = thirdPerson->rotation;
        translation = thirdPerson->translation;
    } else {
        return {};
    }
    return std::pair(QuaternionToEuler(rotation), translation);
}

RayCastResult RayCast::Cast(std::function<bool(RE::NiAVObject*)> const& evaluator, float raySize) {
    auto player = RE::PlayerCharacter::GetSingleton();

    auto [camera_rotation, camera_position] = GetCameraData();

    auto [ray_position, ray_object] = CastRay(player, camera_rotation, camera_position, evaluator, raySize);
    return RayCastResult(ray_position, ray_object);
}

std::pair<RE::NiPoint3, RE::TESObjectREFR*> RayCast::CastRay(
    RE::Actor* caster, RE::NiPoint3 angle, RE::NiPoint3 position,
    std::function<bool(RE::NiAVObject*)> const& evaluator, float raySize) {
    auto havokWorldScale = RE::bhkWorld::GetWorldScale();
    RE::bhkPickData pick_data;
    RE::NiPoint3 ray_start, ray_end;

    ray_start = position;
    ray_end = ray_start + rotate(raySize, angle);
    pick_data.rayInput.from = ray_start * havokWorldScale;
    pick_data.rayInput.to = ray_end * havokWorldScale;

    auto dif = ray_start - ray_end;

    auto collector = RayCollector(evaluator);
    collector.Reset();
    pick_data.rayHitCollectorA8 = reinterpret_cast<RE::hkpClosestRayHitCollector*>(&collector);

    const auto ply = RE::PlayerCharacter::GetSingleton();
    if (!ply->parentCell) return {};

    auto physicsWorld = ply->parentCell->GetbhkWorld();
    if (physicsWorld) {
        physicsWorld->PickObject(pick_data);
    }

    RayCollector::HitResult best = {};
    best.hitFraction = 1.0f;
    RE::NiPoint3 bestPos = {};

    for (auto& hit : collector.GetHits()) {
        const auto pos = (dif * hit.hitFraction) + ray_start;
        if (best.body == nullptr) {
            best = hit;
            bestPos = pos;
            continue;
        }

        if (hit.hitFraction < best.hitFraction) {
            best = hit;
            bestPos = pos;
        }
    }

    if (!best.body) {
        return std::pair(ray_end, nullptr);
    }

    auto hitpos = ray_start + (ray_end - ray_start) * best.hitFraction;

    auto av = best.getAVObject();

    if (av) {
        auto ref = av->GetUserData();

        return std::pair(hitpos, ref);
    }
    return std::pair(hitpos, nullptr);
}
