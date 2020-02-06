#include <exception>
#include <iostream>
#include <string>
#include <stack>

#ifndef HPCPARALLELPATTERN_H
  #include "HPCParallelPattern.h"
#endif

class PInTRuntimeException: public std::exception{
public:
  virtual const char* what() const throw()=0;
};

class TooManyEndsException: public PInTRuntimeException{
public:
  TooManyEndsException(){};
  TooManyEndsException(std::string ID);
  const char* what() const throw();
  void resolveError();
private:
  std::string ID = "ID";
};

class TooManyBeginsException: public PInTRuntimeException{
public:
  TooManyBeginsException(){};
  TooManyBeginsException(std::string ID);
  const char* what() const throw();
  void resolveError();
private:
  std::string ID = "";
};

class PatternSpreadOverSatementsException: public PInTRuntimeException{
public:
  const char* what() const throw();
  void resolveError();
};

/*
 * This exception should only be caught in main function
 */
class TerminateEarlyException: public PInTRuntimeException{
public:
  const char* what() const throw();
};

class WrongNestingException: public PInTRuntimeException{
public:
  WrongNestingException(std::string ID, std::string TopID);
  const char* what() const throw();
private:
  std::string ID;
  std::string TopID;
};

class WrongSyntaxException: public PInTRuntimeException{
public:
  WrongSyntaxException(PatternOccurrence* PatOc);
  const char* what() const throw();
private:
  std::string ID;
};

class missingPatternEnd: public PInTRuntimeException{
public:
  missingPatternEnd(std::vector<PatternCodeRegion*> PatContext);
  missingPatternEnd(){};
  const char* what() const throw();
  void printPatternWithNoEnd() const;
private:
  std::vector<PatternCodeRegion*> PatternVector;

};
