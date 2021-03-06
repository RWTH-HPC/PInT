#include "TestsTest3.h"
#include "PatternInstrumentation.h"

void Test::TestOperatorTypeQualifiers(){

  const int i = 0;
  PatternInstrumentation::Pattern_Begin("FindingConcurrency TypeQualifiers TQ2");
	PatternInstrumentation::Pattern_End("TQ2");
 PatternInstrumentation::Pattern_Begin("FindingConcurrency TypeQualifiers TQ4");
	PatternInstrumentation::Pattern_End("TQ4");

  PatternInstrumentation::Pattern_Begin("FindingConcurrency TypeQualifiers TQ5");
  PatternInstrumentation::Pattern_Begin("FindingConcurrency TypeQualifiers TQ6");

  PatternInstrumentation::Pattern_End("TQ6");
	PatternInstrumentation::Pattern_End("TQ5");

  OtherFunction();

  const int s = 0 , t = 0, d = 0;

  const volatile int q = 0 , r = 0, p = 0;
  volatile int a = 4;
}

void Test::OtherFunction(){
  PatternInstrumentation::Pattern_Begin("FindingConcurrency TypeQualifiers TQ7");
	PatternInstrumentation::Pattern_End("TQ7");
}
