//----------------------------------------------------------------------------
// This class helps to decide if a sequence of 'traceroute' requests is over
// our limit (in rt, 'rt.req' per 'rt.sec' seconds) 
// This is accomplished by keep an array of size 'rt.req+1', which should be 
// treated as a 'circle' recording requests time. 'endIdx' points to the place
// where new times should be written to; 'stIdx' points to 'rt.req' before
// 'endIdx' (endIdx-stIdx) == rt.req 
// If record[endIdx] - record[stIdx] < rt.sec, we are over the limit
//
// Intially, record is filled with 0.0; and stIdx=0, endIdx=rt.req
//========================================
// Weiran Zhao, Computer Science Dept
// Indiana University, Bloomington
//========================================
// Started: Fri,Oct 11th 2013 03:49:17 PM EDT
// Modified: Fri,Oct 11th 2013 04:45:08 PM EDT
//           add a function to return current rate limit
// Last Modified: Fri,Oct 11th 2013 04:49:18 PM EDT
//----------------------------------------------------------------------------
#ifndef _RATE_LIMITING_H_
#define _RATE_LIMITING_H_
#include"progArgs.h"
class rateLimiting {
    private:
        int timeThres;  // time threshold (== rt.sec)
        int recSize;    // size of record
        double *record; // keep requests time for 'r+1' records
        int stIdx;      // point to the 'start' of record
        int endIdx;     // point to the 'end' of record
        char tmp[64];   // used for return 'currentLimit()'

        // return current time from *epoch*
        double elapsedTime();
    public:
        rateLimiting(rate* rt);
        ~rateLimiting();
        // true if this request is over time limit, otherwise, false
        bool isOverLimit();
        // return current limit;
        char* currentLimit();
};
#endif
