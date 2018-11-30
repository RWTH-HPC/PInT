#include "HPCRunningStats.h"

HPCPatternStatistic* actualStats;

void setActualStats(HPCPatternStatistic* actualStatistics[]){
    actualStats = *actualStatistics;
}

HPCPatternStatistic* getActualStats(){
    return actualStats;
}
