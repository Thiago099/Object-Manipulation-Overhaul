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
    return (float)std::sqrt(dx * dx + dy * dy + dz * dz);
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

std::map<std::string, uint64_t> materialIds = {
   {"none", 0ul},
   {"stonebroken", 131151687ul},
   {"blockblade1hand", 165778930ul},
   {"meat", 220124585ul},
   {"carriagewheel", 322207473ul},
   {"metallight", 346811165ul},
   {"woodlight", 365420259ul},
   {"snow", 398949039ul},
   {"gravel", 428587608ul},
   {"chainmetal", 438912228ul},
   {"bottle", 493553910ul},
   {"wood", 500811281ul},
   {"ash", 534864873ul},
   {"skin", 591247106ul},
   {"blockblunt", 593401068ul},
   {"dlc1deerskin", 617099282ul},
   {"insect", 668408902ul},
   {"barrel", 732141076ul},
   {"ceramicmedium", 781661019ul},
   {"basket", 790784366ul},
   {"ice", 873356572ul},
   {"glassstairs", 880200008ul},
   {"stonestairs", 899511101ul},
   {"water", 1024582599ul},
   {"draugrskeleton", 1028101969ul},
   {"blade1hand", 1060167844ul},
   {"book", 1264672850ul},
   {"carpet", 1286705471ul},
   {"metalsolid", 1288358971ul},
   {"axe1hand", 1305674443ul},
   {"blockblade2hand", 1312943906ul},
   {"organiclarge", 1322093133ul},
   {"amulet", 1440721808ul},
   {"woodstairs", 1461712277ul},
   {"mud", 1486385281ul},
   {"bouldersmall", 1550912982ul},
   {"snowstairs", 1560365355ul},
   {"stoneheavy", 1570821952ul},
   {"dragonskeleton", 1574477864ul},
   {"trap", 1591009235ul},
   {"bowsstaves", 1607128641ul},
   {"alduin", 1730220269ul},
   {"blockbowsstaves", 1763418903ul},
   {"woodasstairs", 1803571212ul},
   {"steelgreatsword", 1820198263ul},
   {"grass", 1848600814ul},
   {"boulderlarge", 1885326971ul},
   {"stoneasstairs", 1886078335ul},
   {"blade2hand", 2022742644ul},
   {"bottlesmall", 2025794648ul},
   {"boneactor", 2058949504ul},
   {"sand", 2168343821ul},
   {"metalheavy", 2229413539ul},
   {"dlc1sabrecatpelt", 2290050264ul},
   {"iceform", 2431524493ul},
   {"dragon", 2518321175ul},
   {"blade1handsmall", 2617944780ul},
   {"skinsmall", 2632367422ul},
   {"potspans", 2742858142ul},
   {"skinskeleton", 2821299363ul},
   {"blunt1hand", 2872791301ul},
   {"stonestairsbroken", 2892392795ul},
   {"skinlarge", 2965929619ul},
   {"organic", 2974920155ul},
   {"bone", 3049421844ul},
   {"woodheavy", 3070783559ul},
   {"chain", 3074114406ul},
   {"dirt", 3106094762ul},
   {"ghost", 3312543676ul},
   {"skinmetallarge", 3387452107ul},
   {"blockaxe", 3400476823ul},
   {"armorlight", 3424720541ul},
   {"shieldlight", 3448167928ul},
   {"coin", 3589100606ul},
   {"blockblunt2hand", 3662306947ul},
   {"shieldheavy", 3702389584ul},
   {"armorheavy", 3708432437ul},
   {"arrow", 3725505938ul},
   {"glass", 3739830338ul},
   {"stone", 3741512247ul},
   {"waterpuddle", 3764646153ul},
   {"cloth", 3839073443ul},
   {"skinmetalsmall", 3855001958ul},
   {"ward", 3895166727ul},
   {"web", 3934839107ul},
   {"trailersteelsword", 3941234649ul},
   {"blunt2hand", 3969592277ul},
   {"dlc1swingingbridge", 4239621792ul},
   {"bouldermedium", 4283869410ul}
};

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

RE::MATERIAL_ID Misc::StringToMaterialId(const std::string& str) {
    auto match = ToLowerCase(str);
    auto it = materialIds.find(match);
    if (it != materialIds.end())
        return static_cast<RE::MATERIAL_ID>(it->second);
    else
        return RE::MATERIAL_ID::kNone;
}
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