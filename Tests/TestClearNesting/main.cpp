#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <string>

#include "PatternInstrumentation.h"
#include "TestsTest3.h"


int main(int argc, char* argv[])
{
	bool wahr,falsch;
	//PatternInstrumentation::Pattern_Begin("FindingConcurrency TypeQualifiers TQ50");
	PatternInstrumentation::Pattern_Begin("FindingConcurrency TypeQualifiers TQ1");

	Test::TestOperatorTypeQualifiers();
	//TQ3 is not a child of TQ1
	// TestOperatorTypeQualifiers is also not a child of TQ1
	Test::OtherFunction();
	PatternInstrumentation::Pattern_End("TQ1");
	  const int i = 0;
	return 0;
}
