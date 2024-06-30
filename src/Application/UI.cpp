#include "Application/UI.h"

void UI::Register() {
    if (!SKSEMenuFramework::IsInstalled()) {
        return;
    }
    SKSEMenuFramework::SetSection("Object Manipulation");
    SKSEMenuFramework::AddSectionItem("Main Menu", Menu1::Render);
}
RE::TESObjectREFR* ref;
RE::FormID id;
bool hasId = true;
std::string name = "not found";

void __stdcall UI::Menu1::Render() {
    if (ImGui::Button("Plant")) {
        auto baseObject = RE::TESForm::LookupByID<RE::TESBoundObject>(0xBCF3C);
        ref = RE::PlayerCharacter::GetSingleton()->PlaceObjectAtMe(baseObject, false).get();
        ObjectManipulationManager::StartDraggingObject(ref);
        id = ref->GetFormID();
    }
    if (ImGui::Button("Vase")) {
        auto baseObject = RE::TESForm::LookupByID<RE::TESObjectSTAT>(0x300D271);
        ref = RE::PlayerCharacter::GetSingleton()
            ->PlaceObjectAtMe(baseObject, false).get();
        ObjectManipulationManager::StartDraggingObject(ref);
        id = ref->GetFormID();
    }
    if (ImGui::Button("Delete")) {
        RE::PlayerCharacter::GetSingleton()
            ->PickUpObject(ref, 1, false, false);
    }
    if (ImGui::Button("Check")) {
        if (auto form = RE::TESForm::LookupByID<RE::TESObjectREFR>(id)) {
            hasId = true;
            name = form->GetName();
        } else {
            hasId = false;
            name = "not found";
        }
    }
}
