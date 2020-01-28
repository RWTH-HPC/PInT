#pragma once

class Halstead;

  void setActualHalstead(Halstead* actualHalstead);
  Halstead* getActualHalstead();

  void setCommandArguments(bool onlyPattern, bool noTree, bool useSpecFiles, bool maxTreeDisplayDepth, bool displayCompilationsList, bool printVersion, bool relationTree);
  bool* getonlypattern();
  bool* getnotree();
  bool* getusespecfiles();
  bool* getmaxtreedisplaydepth();
  bool* getdisplaycompilatonslist();
  bool* getprintversion();
  bool* getrelationtree();
