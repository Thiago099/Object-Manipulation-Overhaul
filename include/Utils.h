#include <glm/ext.hpp>
#include <glm/glm.hpp>

namespace Utils {
    std::pair<RE::NiQuaternion, RE::NiPoint3> GetCameraData();
    std::pair<RE::NiPoint3, RE::NiPoint3> PlayerCameraRayPos(std::function<bool(RE::NiAVObject*)> const& evaluator);
    RE::TESObjectREFR* PlayerCameraRayRefr(std::function<bool(RE::NiAVObject*)> const& evaluator, float raySize = 2000000000);
    std::pair<RE::NiPoint3, RE::TESObjectREFR*> RaycastObjectRefr(RE::Actor* caster, RE::NiQuaternion angle,
                                                                  RE::NiPoint3 position,
                                         std::function<bool(RE::NiAVObject*)> const& evaluator, float raySize);
    void SetPosition(RE::TESObjectREFR* ref, const RE::NiPoint3& a_position);
    void SetAngle(RE::TESObjectREFR* ref, const RE::NiPoint3& a_position);

    void StopVisualEffect(RE::TESObjectREFR* r, void* e);
    float DistanceBetweenTwoPoints(RE::NiPoint3& a, RE::NiPoint3& b);

    double NormalizeAngle(double angle_rad);

    RE::NiColorA CreateColor(uint32_t color);

    bool IsStatic(RE::COL_LAYER& col);

    RE::NiObject* GetPlayer3d();

    void MoveTo_Impl(RE::TESObjectREFR* ref, const RE::ObjectRefHandle& a_targetHandle, RE::TESObjectCELL* a_targetCell,
                     RE::TESWorldSpace* a_selfWorldSpace, const RE::NiPoint3& a_position,
                     const RE::NiPoint3& a_rotation);




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

