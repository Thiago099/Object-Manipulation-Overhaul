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
    if (ImGui::Button("Crash")) {
        const auto dataHandler = RE::TESDataHandler::GetSingleton();
        auto shaderFormId = dataHandler->LookupFormID(0x800, "ObjectManipulator.esp");
        auto shader = RE::TESForm::LookupByID<RE::TESEffectShader>(shaderFormId);

       RE::PlayerCharacter::GetSingleton()
            ->As<RE::TESObjectREFR>()
            ->ApplyEffectShader(shader);

               RE::PlayerCharacter::GetSingleton()
            ->As<RE::TESObjectREFR>()
            ->ApplyEffectShader(shader,0);

    }
}