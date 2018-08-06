#include "DesignSpaces.h"



/**
 * @brief Converts a string to the corresponding design space enumeration value.
 *
 * @param str The string to translate.
 *
 * @return Corresponding design space.
 **/
DesignSpace StrToDesignSpace(std::string str)
{
	if (!str.compare("FindingConcurrency")) 
	{
		return FindingConcurrency;
	}
	else if (!str.compare("AlgorithmStructure"))
	{
		return AlgorithmStructure;
	}
	else if (!str.compare("SupportingStructure"))
	{
		return SupportingStructure;	
	}
	else if (!str.compare("ImplementationMechanism"))
	{
		return ImplementationMechanism;
	}
	else
	{
		return Unknown;
	}
}



/**
 * @brief Converts a design space enumeration value to the matching string.
 *
 * @param DesignSp The design space value to be converted.
 *
 * @return String corresponding to the design space.
 **/
std::string DesignSpaceToStr(DesignSpace DesignSp)
{
	if (DesignSp == FindingConcurrency)
	{
		return "FindingConcurrency";
	}
	else if (DesignSp == AlgorithmStructure)
	{
		return "AlgorithmStructure";
	}
	else if (DesignSp == SupportingStructure)
	{
		return "SupportingStructure";
	}
	else if (DesignSp == ImplementationMechanism)
	{
		return "ImplementationMechanism";
	}
	else
	{
		return "Unknown";
	}
}
