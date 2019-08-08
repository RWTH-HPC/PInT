#include <exception>
#include <iostream>

class PInTRuntimeError: public std::exception{
public:
  virtual const char* what() const throw();
};

class TooManyEndsError: public PInTRuntimeError{
public:
  virtual const char* what() const throw();
  void resolveError();
};

class TooManyBegins: public PInTRuntimeError{
public:
  virtual const char* what() const throw();
  void resolveError();
};

class PatternSpreadOverSatements{
public:
  virtual const char* what() const throw();
  void resolveError();
};
