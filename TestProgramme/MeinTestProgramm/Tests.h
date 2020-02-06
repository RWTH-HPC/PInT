class Test;

class OtherClass{
public:
   int uselessFunc();
};

class Test{

public:
  virtual void VirtualFunction();
  static void TestOperatorTypeQualifiers();

  static void OtherFunction();
  friend int OtherClass::uselessFunc();
};
class TestDerived : public Test {

  void VirtualFunction() final;
};
