#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <string>

#include "PatternInstrumentation.h"
#include "Tests.h"


int main(int argc, char* argv[])
{
	bool wahr,falsch;
	//PatternInstrumentation::Pattern_Begin("FindingConcurrency TypeQualifiers TQ50");
	PatternInstrumentation::Pattern_Begin("FindingConcurrency TypeQualifiers TQ1");
	PatternInstrumentation::Pattern_Begin("FindingConcurrency TypeQualifiers TQ11");

	Test::TestOperatorTypeQualifiers();
	//TQ3 is not a child of TQ1
	PatternInstrumentation::Pattern_Begin("FindingConcurrency TypeQualifiers TQ3");
	// TestOperatorTypeQualifiers is also not a child of TQ1
	Test::OtherFunction();
	  const int i = 0;
	return 0;
}
