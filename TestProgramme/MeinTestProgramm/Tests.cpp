#include "Tests.h"

//+1 "::",  +1 "()"
void Test::TestOperatorTypeQualifiers(){
/*
  TypeQualifiers: const, friend, volatile,final
*/
// +1 "const", +1 "="
  const int i = 0;

/*
  Here we have a special case: we defines the number of operants when declares in one
  statement differently.
  const volatile int q = 0 , r = 0, p = 0; for example is treated as

  const volatile int q = 0;
  const volatile int r = 0;
  const volatile int p = 0;

  it does nor matter if u imagine counting the "," or the ";"
  because we count those indirectly while counting VarDecl or DeclStmt etc.

*/

// +3 "const" +3 "="
  const int s = 0 , t = 0, d = 0;

  // +3 "const",   +3 "volatile",   +3 "="
  const volatile int q = 0 , r = 0, p = 0;
// +1 "volatile", +1 "="
  volatile int a = 4;
}
//+1 "::",  +1 "()"
int OtherClass::uselessFunc(){
  return 0;
}
