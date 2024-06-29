#pragma once
#include <glm/ext.hpp>
#include <glm/glm.hpp>
// TODO: Add smooth cam creadits
class RayCollector {
public:
    struct HitResult {
        glm::vec3 normal;
        float hitFraction;
        const RE::hkpCdBody* body;

        RE::NiAVObject* getAVObject();
    };
    std::function<bool(RE::NiAVObject*)> const& evaluator;

public:
    RayCollector(std::function<bool(RE::NiAVObject*)> const& evaluator);
    ~RayCollector() = default;

    virtual void AddRayHit(const RE::hkpCdBody& body, const RE::hkpShapeRayCastCollectorOutput& hitInfo);

    const std::vector<HitResult>& GetHits();

    void Reset();

private:
    float earlyOutHitFraction{1.0f};  // 08
    std::uint32_t pad0C{};
    RE::hkpWorldRayCastOutput rayHit;  // 10

    std::vector<HitResult> hits{};
};
