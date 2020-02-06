#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <string>

#include "PatternInstrumentation.h"
#include "TestsForTrackingEndsAndBegins.h"


int main(int argc, char* argv[])
{
	bool wahr,falsch;

	PatternInstrumentation::Pattern_Begin("FindingConcurrency TypeQualifiers TQ1");

	Test::TestOperatorTypeQualifiers();

	  const int i = 0;
	return 0;
}
