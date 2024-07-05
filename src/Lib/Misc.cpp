#include "Lib/Misc.h"
bool Misc::IsEqual(const std::string& a, const std::string& b) {
    if (a.size() != b.size()) {
        return false;
    }
    return std::equal(a.begin(), a.end(), b.begin(), [](char a, char b) { return std::toupper(a) == std::toupper(b); });
}
void Misc::SetPosition(RE::TESObjectREFR* ref, const RE::NiPoint3& a_position) {
    if (!ref) {
        return;
    }
    using func_t = void(RE::TESObjectREFR*, const RE::NiPoint3&);
    REL::Relocation<func_t> func{RELOCATION_ID(19363, 19790)};
    return func(ref, a_position);
}

void Misc::SetAngle(RE::TESObjectREFR* ref, const RE::NiPoint3& a_position) {
    if (!ref) {
        return;
    }
    using func_t = void(RE::TESObjectREFR*, const RE::NiPoint3&);
    REL::Relocation<func_t> func{RELOCATION_ID(19359, 19786)};
    return func(ref, a_position);
}

float Misc::DistanceBetweenTwoPoints(RE::NiPoint3& a, RE::NiPoint3& b) {
    double dx = b.x - a.x;
    double dy = b.y - a.y;
    double dz = b.z - a.z;
    return std::sqrt(dx * dx + dy * dy + dz * dz);
}

float Misc::NormalizeAngle(float angle) {
    angle = glm::mod(angle + glm::pi<float>(), glm::two_pi<float>());
    if (angle < 0.0f) angle += glm::two_pi<float>();
    return angle - glm::pi<float>();
}

RE::NiColorA Misc::CreateColor(uint32_t color) {
    // Extracting RGB components from the hex value
    float red = ((color & 0xFF000000) >> 24) / 255.0f;
    float green = ((color & 0x00FF0000) >> 16) / 255.0f;
    float blue = ((color & 0x0000FF00) >> 8) / 255.0f;
    float alpha = (color & 0x000000FF) / 255.0f;

    return RE::NiColorA(red, green, blue, alpha);
}

bool Misc::IsStatic(RE::COL_LAYER& col) {
    // logger::trace("col: {}", static_cast<int>(col));
    switch (col) {
        case RE::COL_LAYER::kUnidentified:
        case RE::COL_LAYER::kStatic:
        case RE::COL_LAYER::kTrees:
        case RE::COL_LAYER::kAnimStatic:
        case RE::COL_LAYER::kTerrain:
        case RE::COL_LAYER::kTrap:
        case RE::COL_LAYER::kGround:
        case RE::COL_LAYER::kPortal:
            return true;
        default:
            return false;
    }
}

RE::NiObject* Misc::GetPlayer3d() {
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

void Misc::MoveTo_Impl(RE::TESObjectREFR* ref, const RE::ObjectRefHandle& a_targetHandle,
                        RE::TESObjectCELL* a_targetCell, RE::TESWorldSpace* a_selfWorldSpace,
                        const RE::NiPoint3& a_position, const RE::NiPoint3& a_rotation) {
    using func_t = decltype(&MoveTo_Impl);
    REL::Relocation<func_t> func{RE::Offset::TESObjectREFR::MoveTo};
    return func(ref, a_targetHandle, a_targetCell, a_selfWorldSpace, a_position, a_rotation);
}
// https://ck.uesp.net/wiki/GetMappedKey_-_Input
// Activate
// Auto-Move
// Back
// CameraPath
// Console
// Favorites
// Forward
// Hotkey1
// Hotkey2
// Hotkey3
// Hotkey4
// Hotkey5
// Hotkey6
// Hotkey7
// Hotkey8
// Journal
// Jump
// Left Attack/Block
// Look
// Move
// Multi-Screenshot
// Pause
// Quick Inventory
// Quick Magic
// Quick Map
// Quick Stats
// Quickload
// Quicksave
// Ready Weapon
// Right Attack/Block
// Run
// Screenshot
// Shout
// Sneak
// Sprint
// Strafe Left
// Strafe Right
// Toggle Always Run
// Toggle POV
// Tween Menu
// Wait
// Zoom In
// Zoom Out
bool Misc::DoesButtonTriggerAction(RE::ButtonEvent* button, std::string action) {
    auto control = RE::ControlMap::GetSingleton();
    if (control) {

        if (button->GetDevice() == RE::INPUT_DEVICE::kKeyboard) {
            auto key = static_cast<RE::BSKeyboardDevice::Key>(button->GetIDCode());
            if (key == static_cast<RE::BSKeyboardDevice::Key>(
                            control->GetMappedKey(action, RE::INPUT_DEVICE::kKeyboard))) {
                return true;
            }
        }
        if (button->GetDevice() == RE::INPUT_DEVICE::kGamepad) {
            auto key = static_cast<RE::BSWin32GamepadDevice::Key>(button->GetIDCode());
            if (key == static_cast<RE::BSWin32GamepadDevice::Key>(
                            control->GetMappedKey(action, RE::INPUT_DEVICE::kGamepad))) {
                return true;
            }
        }
        if (button->GetDevice() == RE::INPUT_DEVICE::kMouse) {
            auto key = static_cast<RE::BSWin32MouseDevice::Key>(button->GetIDCode());
            if (key == static_cast<RE::BSWin32MouseDevice::Key>(
                           control->GetMappedKey(action, RE::INPUT_DEVICE::kMouse))) {
                return true;
            }
        }
    }
    return false;
}

std::vector<std::string> formTypes =
    {
        "none",
        "plugininfo",
        "formgroup",
        "gamesetting",
        "keyword",
        "locationreftype",
        "action",
        "textureset",
        "menuicon",
        "global",
        "class",
        "faction",
        "headpart",
        "eyes",
        "race",
        "sound",
        "acousticspace",
        "skill",
        "magiceffect",
        "script",
        "landtexture",
        "enchantment",
        "spell",
        "scroll",
        "activator",
        "talkingactivator",
        "armor",
        "book",
        "container",
        "door",
        "ingredient",
        "light",
        "misc",
        "apparatus",
        "static",
        "staticcollection",
        "movablestatic",
        "grass",
        "tree",
        "flora",
        "furniture",
        "weapon",
        "ammo",
        "npc",
        "levelednpc",
        "keymaster",
        "alchemyitem",
        "idlemarker",
        "note",
        "constructibleobject",
        "projectile",
        "hazard",
        "soulgem",
        "leveleditem",
        "weather",
        "climate",
        "shaderparticlegeometrydata",
        "referenceeffect",
        "region",
        "navigation",
        "cell",
        "reference",
        "actorcharacter",
        "projectilemissile",
        "projectilearrow",
        "projectilegrenade",
        "projectilebeam",
        "projectileflame",
        "projectilecone",
        "projectilebarrier",
        "placedhazard",
        "worldspace",
        "land",
        "navmesh",
        "tlod",
        "dialogue",
        "info",
        "quest",
        "idle",
        "package",
        "combatstyle",
        "loadscreen",
        "leveledspell",
        "animatedobject",
        "water",
        "effectshader",
        "toft",
        "explosion",
        "debris",
        "imagespace",
        "imageadapter",
        "formlist",
        "perk",
        "bodypartdata",
        "addonnode",
        "actorvalueinfo",
        "camerashot",
        "camerapath",
        "voicetype",
        "materialtype",
        "impact",
        "impactdataset",
        "armature",
        "encounterzone",
        "location",
        "message",
        "ragdoll",
        "defaultobject",
        "lightingmaster",
        "musictype",
        "footstep",
        "footstepset",
        "storymanagerbranchnode",
        "storymanagerquestnode",
        "storymanagereventnode",
        "dialoguebranch",
        "musictrack",
        "dialogueview",
        "wordofpower",
        "shout",
        "equipslot",
        "relationship",
        "scene",
        "associationtype",
        "outfit",
        "artobject",
        "materialobject",
        "movementtype",
        "soundrecord",
        "dualcastdata",
        "soundcategory",
        "soundoutputmodel",
        "collisionlayer",
        "colorform",
        "reverbparam",
        "lensflare",
        "lenssprite",
        "volumetriclighting",
};


RE::FormType Misc::StringToFormType(const std::string& str) {
    auto match = ToLowerCase(str);
    auto it = std::find(formTypes.begin(), formTypes.end(), match);
    if (it != formTypes.end())
        return static_cast<RE::FormType>(distance(formTypes.begin(), it)); 
    else
        return RE::FormType::None;  
}
std::string Misc::ToLowerCase(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) { return std::tolower(c); });
    return result;
}