#include "HPCError.h"


TooManyEndsException::TooManyEndsException(std::string ID){
  this->ID = ID;
}
const char* TooManyEndsException::what() const throw(){

  std::string s = "You probably added one end of a patten to much.\n " + this->ID+ " ends outside of any Pattern.";
  std::cout << s << '\n';

  return s.c_str();
};

TooManyBeginsException::TooManyBeginsException(std::string ID){
  this->ID = ID;
}
const char* TooManyBeginsException::what() const throw(){
  std::cout << "\033[31mYou have used more than one Pattern_Begin with the same ID, "<< this->ID << ".\n\033[0m";
  return "You have used more than one begin with the same ID, ";
};

const char* PatternSpreadOverSatementsException::what() const throw(){
  return "You spread your patten over if, if-else, switch-case, while, for etc. statements. Please begin AND end your pattern eather inside or outside of the statement or loop";
};

const char* TerminateEarlyException:: what() const throw(){
  return "An error occured. We could not resolve so we termate early. The statistics are not usable.";
};

WrongNestingException::WrongNestingException(std::string ID, std::string TopID){
  this->ID=ID;
  this->TopID=TopID;
};

const char* WrongNestingException::what() const throw(){
  std::string s = "\033[31mInconsistency in the pattern stack detected. Check the structure of the instrumentation in the application code!\nYou probably tried to end " + this->ID + " before ending " + this->TopID + "\033[0m";
  std::cout << s << std::endl;
  return s.c_str();
};

WrongSyntaxException::WrongSyntaxException(PatternOccurrence* PatOc){
  this->ID = PatOc->GetID();
};

const char* WrongSyntaxException::what() const throw(){
  std::string str ="Pattern Occurrences with same identifier have different underlying pattern:" + this->ID;
  std::cout << str << std::endl;
  //std::cout << this->ID << std::endl;
  return str.c_str();
};
