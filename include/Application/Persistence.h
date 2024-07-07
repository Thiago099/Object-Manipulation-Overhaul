#pragma once
#include "Application/ObjectManipulationManager.h"

namespace Persistence {
    void SaveCallback(SKSE::SerializationInterface* a_intfc);
    void LoadCallback(SKSE::SerializationInterface* a_intfc);
}