#include <exception>
#include <iostream>

class PInTRuntimeError: public std::exception{
public:
  virtual const char* what() const throw();
};

class TooManyEndsError: public PInTRuntimeError{
public:
  const char* what() const throw();
  void resolveError();
};

class TooManyBeginsError: public PInTRuntimeError{
public:
  const char* what() const throw();
  void resolveError();
};

class PatternSpreadOverSatements{
public:
  const char* what() const throw();
  virtual void resolveError();
};
