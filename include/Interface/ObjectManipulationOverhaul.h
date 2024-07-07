#pragma once

#include "Application/ObjectManipulationManager.h"

#define FUNCTION_PREFIX extern "C" [[maybe_unused]] __declspec(dllexport)

FUNCTION_PREFIX void StartDraggingObject(RE::TESObjectREFR* object);
