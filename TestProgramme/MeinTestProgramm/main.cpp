#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <vector>
#include <string>
#include <iostream>

#include "PatternInstrumentation.h"
#include "Tests.h"


int main(int argc, char* argv[])
{
	// is not consideres because it isn't inside a pattern
	bool wahr,falsch;


	PatternInstrumentation::Pattern_Begin("FindingConcurrency TypeQualifiers TQ1");
	/* int, float, char,double, long, short, signed, unsigned, void
	*/
	  const int i = 0;
	//+1 "::",  +1 "()"
	Test::TestOperatorTypeQualifiers();

	PatternInstrumentation::Pattern_End("TQ1");
	return 0;
}
