#pragma once
#include "Application/ObjectManipulationManager.h"
#include "Lib/Regex.h"
#include "Lib/File.h"

class InputManagerConfiguration {
    static inline Regex parametersRegex = Regex("\\s*([^,\\s]+)\\s*(?:,|$)");
	public:
    static void Install(std::string path);
};