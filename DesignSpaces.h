#pragma once

#include <string>



/**
 * Design Spaces
 */
enum DesignSpace { Unknown, FindingConcurrency, AlgorithmStructure, SupportingStructure, ImplementationMechanism };

DesignSpace StrToDesignSpace(std::string str);

std::string DesignSpaceToStr(DesignSpace DesignSp);
