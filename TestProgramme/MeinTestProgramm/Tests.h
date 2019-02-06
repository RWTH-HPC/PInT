//+1 "{}"
class Test{

public:
  //+1 "()",   +1 "virtual"
  virtual void VirtualFunction();
  //+1 "()"
  void TestOperatorTypeQualifiers();
  // +1 "friend",   +1 "::",   +1 "()"
  friend void OtherClass::uselessFunc();
};

//+1 "{}"
class OtherClass{
public:
  //+1 "()"
   void uselessFunc();
};

// +1 ":",   +1 "()",   +1 "{}"
class TestDerived : Test(){
  //+1 "final",   +1 "()"
  final VirtualFunction();
}
