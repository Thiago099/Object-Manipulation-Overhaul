#include "Application/Persistence.h"
std::mutex callbackMutext;

void Persistence::SaveCallback(SKSE::SerializationInterface* a_intfc) {
    std::lock_guard<std::mutex> lock(callbackMutext);
    try {
        logger::trace("save started");
        if (a_intfc->OpenRecord('PICK', 1)) {
            if (ObjectManipulationManager::IsDragging()) {
                logger::trace("dragging");
                if (auto form = ObjectManipulationManager::GetDragObject()) {
                    a_intfc->WriteRecordData(form->GetFormID());
                }
            } else {
                logger::trace("not dragging");
                a_intfc->WriteRecordData(static_cast<RE::FormID>(0));
            }
        } else {
            logger::error("Failed to open record for pick!");
        }
        logger::trace("save done");
    } catch (const std::exception&) {
        logger::trace("error saving");
    }
}

void Persistence::LoadCallback(SKSE::SerializationInterface* a_intfc) {
    std::lock_guard<std::mutex> lock(callbackMutext);
    try {
        logger::trace("load started");
        uint32_t type;
        uint32_t version;
        uint32_t length;
        while (a_intfc->GetNextRecordInfo(type, version, length)) {
            switch (type) {
                case 'PICK': {
                    uint32_t item;
                    if (a_intfc->ReadRecordData(item)) {
                        if (item != 0) {
                            if (auto form = RE::TESForm::LookupByID<RE::TESObjectREFR>(item)) {
                                ObjectManipulationManager::StartDraggingObject(form);
                                logger::trace("dragging restored {}", form->GetName());
                            } else {
                                logger::trace("form not found");
                            }
                        } else {
                            logger::trace("was not draggign");
                        }
                    }
                } break;
                default:
                    logger::trace("unknown obj");
                    break;
            }
        }
        logger::trace("load done");
    } catch (const std::exception&) {
        logger::trace("error loading");
    }
}