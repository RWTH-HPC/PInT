#pragma once

#include <string>



/*! Design Spaces as defined in the literature. */
enum DesignSpace { Unknown /*!< Use this to neglect design spaces completely or if the design space is really not categorisable. */, FindingConcurrency, AlgorithmStructure, SupportingStructure, ImplementationMechanism };

DesignSpace StrToDesignSpace(std::string str);

std::string DesignSpaceToStr(DesignSpace DesignSp);
