#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <string>

#include "PatternInstrumentation.h"
#include "TestsRecusion.h"


int main(int argc, char* argv[])
{
	bool wahr,falsch;

	PatternInstrumentation::Pattern_Begin("FindingConcurrency TypeQualifiers TQ1");

	Test::TestOperatorTypeQualifiers();

	PatternInstrumentation::Pattern_End("TQ1");

	  const int i = 0;

Test::TestOperatorTypeQualifiers();
	return 0;
}
