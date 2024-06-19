#include "UI.h"

void UI::Register() {

    if (!SKSEMenuFramework::IsInstalled()) {
        return;
    }
    SKSEMenuFramework::SetSection("Object Manipulation");
    SKSEMenuFramework::AddSectionItem("Main Menu", Menu1::Render);
}

void __stdcall UI::Menu1::Render() { 
    if (ImGui::Button("Create")) {
        auto baseObject = RE::TESForm::LookupByID<RE::TESBoundObject>(0x15bab);
        auto objectRef = RE::PlayerCharacter::GetSingleton()->PlaceObjectAtMe(baseObject, true);
        auto object = objectRef.get();
        ObjectManipulationManager::Pick(object);
    }
}