#include "HPCRunningStats.h"

Halstead* actualHal;

/*CommandLine arguments */
bool* notree;
bool* relationtree;
bool* onlypattern;
bool* usespecfiles;
bool* maxtreedisplaydepth;
bool* displaycompilatonslist;
bool* printversion;


void setActualHalstead(Halstead* actualHalstead){
    actualHal = actualHalstead;
}

Halstead* getActualHalstead(){
    return actualHal;
}

void setRelationTree(bool value){
  relationtree = &value;
}

void setNoTree(bool value){
  notree = &value;
}

void setCommandArguments(bool onlyPattern, bool noTree, bool useSpecFiles, bool maxTreeDisplayDepth, bool displayCompilationsList, bool printVersion, bool relationTree){
  notree = &noTree;
  usespecfiles = &useSpecFiles;
  onlypattern = &onlyPattern;
  maxtreedisplaydepth = &maxTreeDisplayDepth;
  displaycompilatonslist = &displayCompilationsList;
  printversion = &printVersion;
  relationtree = &relationTree;
}
