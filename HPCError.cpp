#include "HPCError.h"

const char* TooManyEndsError::what() const throw(){
  return "You have used too many ends with the same ID";
};

const char* TooManyBeginsError::what() const throw(){
  return "Your have used more than one begin with the same ID";
};

const char* PatternSpreadOverSatements::what() const throw(){
  return "You spread your patten over if, if-else, switch-case, while, for etc. statements. Please begin AND end your pattern eather inside or outside of the statement or loop";
}; 
