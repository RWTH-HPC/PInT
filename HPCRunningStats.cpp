#include "HPCRunningStats.h"

Halstead* actualHal;

void setActualHalstead(Halstead* actualHalstead){
    actualHal = actualHalstead;
}

Halstead* getActualHalstead(){
    return actualHal;
}
