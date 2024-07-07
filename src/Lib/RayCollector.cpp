#include "Lib/RayCollector.h"

RayCollector::RayCollector(std::function<bool(RE::NiAVObject*)> const& evaluator) : evaluator(evaluator) {}

void RayCollector::AddRayHit(const RE::hkpCdBody& body, const RE::hkpShapeRayCastCollectorOutput& hitInfo) {
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

    //logger::info("Flags: {}", m);

    constexpr uint64_t filter = 0x40122736 | 0x8000;
    if ((m & filter) != 0) {
        if (!evaluator(hit.getAVObject())) {
            return;
        }
        earlyOutHitFraction = hit.hitFraction;
        hits.push_back(std::move(hit));
    }
}

const std::vector<RayCollector::HitResult>& RayCollector::GetHits() { return hits; }

void RayCollector::Reset() {
    earlyOutHitFraction = 1.0f;
    hits.clear();
}

RE::NiAVObject* RayCollector::HitResult::getAVObject() {
    typedef RE::NiAVObject* (*_GetUserData)(const RE::hkpCdBody*);
    static auto getAVObject = REL::Relocation<_GetUserData>(RELOCATION_ID(76160, 77988));
    return body ? getAVObject(body) : nullptr;
}
