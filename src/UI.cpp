#include "UI.h"

void UI::Register() {

    if (!SKSEMenuFramework::IsInstalled()) {
        return;
    }
    SKSEMenuFramework::SetSection("Thiago Template");
    SKSEMenuFramework::AddSectionItem("Menu 1", Menu1::Render);
}

void __stdcall UI::Menu1::Render() { 
    FontAwesome::PushSolid();
    Font::PushBig();
    ImGui::Text((FontAwesome::UnicodeToUtf8(0xf03e) + "Hello World").c_str());
    Font::PushDefault();
    ImGui::Text((FontAwesome::UnicodeToUtf8(0xf03e) + "Hello World").c_str());
    Font::PushSmall();
    ImGui::Text((FontAwesome::UnicodeToUtf8(0xf03e) + "Hello World").c_str());
    Font::Clear();
    ImGui::Text((FontAwesome::UnicodeToUtf8(0xf03e) + "Hello World").c_str());
}