#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <string>

#include "PatternInstrumentation.h"
#include "TestsCorrectChildRegistration.h"


int main(int argc, char* argv[])
{
	bool wahr,falsch;

	PatternInstrumentation::Pattern_Begin("FindingConcurrency TypeQualifiers TQ1");

	Test::TestOperatorTypeQualifiers();
	//this is not a child of TQ1
	PatternInstrumentation::Pattern_Begin("FindingConcurrency TypeQualifiers TQ3");
	 PatternInstrumentation::Pattern_End("TQ3");
	  const int i = 0;
	return 0;
}
