#include "Utils.h"

#define M_PI 3.14159265358979323846

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


std::pair<RE::NiQuaternion, RE::NiPoint3> Utils::GetCameraData() {
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
    } else if (camera->currentState.get()->id == RE::CameraState::kThirdPerson) {
        rotation = thirdPerson->rotation;
        translation = thirdPerson->translation;
    } 
    else {
        return {};
    }
    return std::pair(rotation, translation);
}

std::pair<RE::NiPoint3, RE::NiPoint3> Utils::PlayerCameraRayPos(std::function<bool(RE::NiAVObject*)> const& evaluator) {

    auto player = RE::PlayerCharacter::GetSingleton();

    auto [rotation, pos] = GetCameraData();

    auto [pos2, refr] = Utils::RaycastObjectRefr(
        player, rotation, pos, evaluator, 2000000000);

    return std::pair<RE::NiPoint3, RE::NiPoint3>(pos, pos2);
}
RE::TESObjectREFR* Utils::PlayerCameraRayRefr(std::function<bool(RE::NiAVObject*)> const& evaluator, float raySize) {
    auto player = RE::PlayerCharacter::GetSingleton();

    auto [rotation, pos] = GetCameraData();

    auto [pos2, refr] = Utils::RaycastObjectRefr(player, rotation, pos, evaluator, raySize);
    return refr;
}

std::pair<RE::NiPoint3, RE::TESObjectREFR*> Utils::RaycastObjectRefr(RE::Actor* caster, RE::NiQuaternion angle, RE::NiPoint3 position,
                                            std::function<bool(RE::NiAVObject*)> const& evaluator, float raySize) {
    auto havokWorldScale = RE::bhkWorld::GetWorldScale();
    RE::bhkPickData pick_data;
    RE::NiPoint3 ray_start, ray_end;

    ray_start = position;
    ray_end = ray_start + rotate(raySize, QuaternionToEuler(angle));
    pick_data.rayInput.from = ray_start * havokWorldScale;
    pick_data.rayInput.to = ray_end * havokWorldScale;

    auto dif = ray_start - ray_end;

    auto collector = Utils::RayCollector(evaluator);
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
    using func_t = void(void*, RE::TESObjectREFR*, void*);
    REL::Relocation<func_t> func{RELOCATION_ID(40381, 41395)};
    return func(singleton.get(), r, ptr);
}

float Utils::DistanceBetweenTwoPoints(RE::NiPoint3& a, RE::NiPoint3& b) {
    double dx = b.x - a.x;
    double dy = b.y - a.y;
    double dz = b.z - a.z;
    return std::sqrt(dx * dx + dy * dy + dz * dz);
}








RE::NiColorA Utils::CreateColor(uint32_t color) {
    // Extracting RGB components from the hex value
    float red = ((color & 0xFF000000) >> 24) / 255.0f;
    float green = ((color & 0x00FF0000) >> 16) / 255.0f;
    float blue = ((color & 0x0000FF00) >> 8) / 255.0f;
    float alpha = (color & 0x000000FF) / 255.0f;

    return RE::NiColorA(red, green, blue, alpha);
}






RE::NiObject* Utils::GetPlayer3d() {
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




void Utils::MoveTo_Impl(RE::TESObjectREFR* ref, const RE::ObjectRefHandle& a_targetHandle, RE::TESObjectCELL* a_targetCell,
                 RE::TESWorldSpace* a_selfWorldSpace, const RE::NiPoint3& a_position, const RE::NiPoint3& a_rotation) {
    using func_t = decltype(&MoveTo_Impl);
    REL::Relocation<func_t> func{RE::Offset::TESObjectREFR::MoveTo};
    return func(ref, a_targetHandle, a_targetCell, a_selfWorldSpace, a_position, a_rotation);
}











Utils::RayCollector::RayCollector(std::function<bool(RE::NiAVObject*)> const& evaluator)
    : evaluator(evaluator){}

void Utils::RayCollector::AddRayHit(const RE::hkpCdBody& body, const RE::hkpShapeRayCastCollectorOutput& hitInfo) {
    HitResult hit;
    hit.hitFraction = hitInfo.hitFraction;
    hit.normal = {hitInfo.normal.quad.m128_f32[0], hitInfo.normal.quad.m128_f32[1], hitInfo.normal.quad.m128_f32[2]};

    const RE::hkpCdBody* obj = &body;
    while (obj) {
        if (!obj->parent) break;
        obj = obj->parent;
    }

    hit.body = obj;
    if (!hit.body) return;

    const auto collisionObj = static_cast<const RE::hkpCollidable*>(hit.body);
    const auto flags = collisionObj->broadPhaseHandle.collisionFilterInfo;

    const uint64_t m = 1ULL << static_cast<uint64_t>(flags);
    constexpr uint64_t filter = 0x40122716;  //@TODO
    if ((m & filter) != 0) {
        if (!evaluator(hit.getAVObject())) {
            return;
        }
        earlyOutHitFraction = hit.hitFraction;
        hits.push_back(std::move(hit));
    }


}

const std::vector<Utils::RayCollector::HitResult>& Utils::RayCollector::GetHits() { return hits; }

void Utils::RayCollector::Reset() {
    earlyOutHitFraction = 1.0f;
    hits.clear();
}

RE::NiAVObject* Utils::RayCollector::HitResult::getAVObject() {
    typedef RE::NiAVObject* (*_GetUserData)(const RE::hkpCdBody*);
    static auto getAVObject = REL::Relocation<_GetUserData>(RELOCATION_ID(76160, 77988));
    return body ? getAVObject(body) : nullptr;
}


