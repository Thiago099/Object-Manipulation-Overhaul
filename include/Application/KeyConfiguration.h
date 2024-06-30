#pragma once
#include "Application/ObjectManipulationManager.h"
#include "Lib/Regex.h"

class KeyConfiguration {
    static inline Regex parametersRegex = Regex("^\\s*([^,\\s]+)\\s*(?:,|$)");
	public:
    static void Install(std::string path);
};