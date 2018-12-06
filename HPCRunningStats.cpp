#include "HPCRunningStats.h"

Halstead* actualHal;

void setHalsteadActualStat(Halstead* actualHalstead){
    actualHal = actualHalstead;
    printf("actuelle statistik gesetzt 'runningStsts:setActualStats'\n");
}

Halstead* getActualHalstead(){
    return actualHal;
}
