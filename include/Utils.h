#include <glm/ext.hpp>
#include <glm/glm.hpp>

namespace Utils {
    std::pair<RE::NiQuaternion, RE::NiPoint3> GetCameraData();
    std::pair<RE::NiPoint3, RE::NiPoint3> PlayerCameraRayPos();
    RE::TESObjectREFR* PlayerCameraRayRefr(std::function<bool(RE::NiAVObject*)> const& evaluator, float raySize = 2000000000);
    RE::NiPoint3 Raycast(RE::Actor* caster, RE::NiQuaternion angle, RE::NiPoint3 position);
    RE::TESObjectREFR* RaycastObjectRefr(RE::Actor* caster, RE::NiQuaternion angle, RE::NiPoint3 position,
                                         std::function<bool(RE::NiAVObject*)> const& evaluator, float raySize);
    void SetPosition(RE::TESObjectREFR* ref, const RE::NiPoint3& a_position);
    void SetAngle(RE::TESObjectREFR* ref, const RE::NiPoint3& a_position);

    void StopVisualEffect(RE::TESObjectREFR* r, void* e);
    float DistanceBetweenTwoPoints(RE::NiPoint3& a, RE::NiPoint3& b);





    //TODO: Add smooth cam creadits
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





}

