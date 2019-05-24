#include "external-function.h"
#include "PatternInstrumentation.h"

void external_function()
{
	int y = 0;
	PatternInstrumentation::Pattern_Begin("ImplementationMechanism VariableIncrement Increment22112");
	y++;
	PatternInstrumentation::Pattern_End("Increment22112");
}
