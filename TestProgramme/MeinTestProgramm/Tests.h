class Test;
//+1 "{}"
class OtherClass{
public:
  //+1 "()"
   int uselessFunc();
};

//+1 "{}"
class Test{

public:
  //+1 "()",   +1 "virtual"
  virtual void VirtualFunction();
  //+1 "()"
  static void TestOperatorTypeQualifiers();

  void OtherFunction();
  // +1 "friend",   +1 "::",   +1 "()"
  friend int OtherClass::uselessFunc();
};
// +1 ":",   +1 "()",   +1 "{}"
class TestDerived : public Test {
  //+1 "final",   +1 "()"
  void VirtualFunction() final;
};
