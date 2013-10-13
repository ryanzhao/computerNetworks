//----------------------------------------------------------------------------
// Check 'rateLimiting.h' for details
//========================================
// Weiran Zhao, Computer Science Dept
// Indiana University, Bloomington
//========================================
// Started: Fri,Oct 11th 2013 03:49:17 PM EDT
// Last Modified: Fri,Oct 11th 2013 04:49:42 PM EDT
//----------------------------------------------------------------------------
#include"rateLimiting.h"
#include<sys/time.h>
#include<cstring>
rateLimiting::rateLimiting(rate* rt) {
    timeThres = rt->getSec();
    recSize = rt->getReq();
    record = new double[recSize];
    stIdx = 0;
    endIdx = recSize-1;
}

rateLimiting::~rateLimiting() {
    delete record;
}

double rateLimiting::elapsedTime() {
    struct timeval t;
    double total;
    gettimeofday(&t, NULL);
    
    total = (double) (t.tv_sec) + 1.0e-6*(double) (t.tv_usec);
    if (total < 0) {
        return(-17.0);
    } else {
        return total;
    }
}

bool rateLimiting::isOverLimit() {
    record[endIdx] = elapsedTime();
    bool res = (record[endIdx] - record[stIdx]) < timeThres ? true:false;
    endIdx = (endIdx+1)%recSize;
    stIdx = (stIdx+1)%recSize;
    return res;
}

char* rateLimiting::currentLimit() {
    memset(tmp, 0, 64);
    sprintf(tmp,"current limit is: %d requests per %d seconds", recSize-1, 
            timeThres);
    return tmp;
}
