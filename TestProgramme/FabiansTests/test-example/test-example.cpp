#include <iostream>

#include "PatternInstrumentation.h"
#include "external-function.h"

int x = 0;


void internal_function();

void indirect_function1();

void indirect_function2();

void recursive_function();

int main (int argc, char** argv)
{

	PatternInstrumentation::Pattern_Begin("FindingConcurrency TaskDecomposition Decomp1");

	// Hallo, ich bin ein Kommentar!
	// Ich auch
	internal_function();

	int y = 0;

	PatternInstrumentation::Pattern_Begin("AlgorithmStructure ForLoop ForLoop1");

	#pragma omp parallel for
	for (int i = 1; i < 10000; i++)
	{
		PatternInstrumentation::Pattern_Begin("ImplementationMechanism VariableIncrement Increment1");
		y++;
		PatternInstrumentation::Pattern_End("Increment1");
		internal_function();
		external_function();
	}
	PatternInstrumentation::Pattern_End("ForLoop1");

	PatternInstrumentation::Pattern_Begin("AlgorithmStructure Indirection Indirection1");
	indirect_function1();
	PatternInstrumentation::Pattern_End("Indirection1");

	PatternInstrumentation::Pattern_End("Decomp1");

	internal_function();

	recursive_function();
}


void internal_function()
{
	PatternInstrumentation::Pattern_Begin("ImplementationMechanism VariableIncrement Increment2");
	x++;
	PatternInstrumentation::Pattern_End("Increment2");
	external_function();

}

void indirect_function1()
{
	indirect_function2();
}

void indirect_function2()
{
	PatternInstrumentation::Pattern_Begin("ImplementationMechanism VariableIncrement IndirectIncrement");
	x++;
	PatternInstrumentation::Pattern_End("IndirectIncrement");
}

void recursive_function()
{
	PatternInstrumentation::Pattern_Begin("AlgorithmStructure Recursion Recursion1");
	recursive_function();
	PatternInstrumentation::Pattern_End("Recursion1");
}
