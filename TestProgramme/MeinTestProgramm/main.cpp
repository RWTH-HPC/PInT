#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <vector>
#include <string>
#include <iostream>

#include "PatternInstrumentation.h"
#include "Tests.h"


int main_(int argc, char* argv[])
{
	// is not consideres because it isn't inside a pattern
	bool wahr,falsch;


	PatternInstrumentation::Pattern_Begin("FindingConcurrency TypeQualifiers TQ1");
	/* int, float, char,double, long, short, signed, unsigned, void
	*/
	  const int i = 0;
	Test::TestOperatorTypeQualifiers();

	PatternInstrumentation::Pattern_End("TQ1");
	const int temp = 0;
	// +0 (Operators)
	int a[100];

	//+1 for "for", +1 for "=", +1 for "<", +1 for "++" (Operators) total 7
	for (int i = 1; i < 101 ;i++) {
		if(i > 5 && i <7)
		{
			break;
		}
	}
	return 0;
}
