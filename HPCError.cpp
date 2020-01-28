#include "HPCError.h"
//#include "HPCRunningStats.h"

TooManyEndsException::TooManyEndsException(std::string ID){
  this->ID = ID;
}
const char* TooManyEndsException::what() const throw(){
std::string s= this->ID;
std::cout<<"\033[31mYou probably added one end of a patten to much.\n" + s;
return " ends outside of any Pattern.\033[0m ";
}

TooManyBeginsException::TooManyBeginsException(std::string ID){
  this->ID = ID;
}
const char* TooManyBeginsException::what() const throw(){
  std::cout << "\033[31mYou have eather used more than one Pattern_Begin with the same ID, "<< this->ID << " or you forgott to end this pattern.\n\033[0m";
  return "You have used more than one begin with the same ID, ";
};

const char* PatternSpreadOverSatementsException::what() const throw(){
  return "You spread your patten over if, if-else, switch-case, while, for etc. statements. Please begin AND end your pattern eather inside or outside of the statement or loop";
};

const char* TerminateEarlyException:: what() const throw(){
  return "An error occured. We could not resolve.We termate early. The statistics are not usable.";
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
  std::string str ="\033[31mPattern Occurrences with same identifier have different underlying pattern:" + this->ID+"\033[0m";
  std::cout << str << std::endl;
  //std::cout << this->ID << std::endl;
  return str.c_str();
};

missingPatternEnd::missingPatternEnd(std::vector<PatternCodeRegion*> PatContext){
  this->PatternVector = PatContext;
};

const char* missingPatternEnd::what() const throw(){
  std::cout << "\n\033[31mYou forgot to end the following PatternCodeRegions: "<< "\n\n";
  this->printPatternWithNoEnd();
  std::cout << "\033[0m" << '\n';
  return "";
};

void missingPatternEnd::printPatternWithNoEnd() const{
  std::vector<PatternCodeRegion*> tempStack = this->PatternVector;
  if(!(tempStack.empty())){
    PatternCodeRegion* PatCodeReg;
    for(unsigned long i=0; i< tempStack.size(); i++){
      PatCodeReg = tempStack.back();
      PatCodeReg->Print();
      std::cout <<'\n';
      tempStack.pop_back();
    }
  }
};
