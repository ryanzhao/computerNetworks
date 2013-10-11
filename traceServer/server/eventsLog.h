//----------------------------------------------------------------------------
// A log class keep track of traceServer activity and log abnormal client usage
// or requests.
//========================================
// Weiran Zhao, Computer Science Dept
// Indiana University, Bloomington
//========================================
// Started: a week or so before Tue,Sep 17th 2013 05:46:20 PM EDT
// Modified: Fri,Oct 11th 2013 12:16:09 PM EDT
//           Modify logIt() function, let it take arguments of variable length
//           Also has to change to use FILE stream instead of ofstream
// Last Modified: Fri,Oct 11th 2013 12:50:42 PM EDT
//----------------------------------------------------------------------------
#ifndef EVENTS_LOG_H_
#define EVENTS_LOG_H_
#include<cstdio>
#include<cstring>
#include<cstdarg>
#include<ctime>
using namespace std;
class eventsLog {
    private:
        FILE* ofile;
        char timeStampBuff[20];
    public:
        eventsLog();
        ~eventsLog();
        // log a message into 'log.txt'
        void logIt(const char* format, ...);
        // return a timestamp of format MM/DD/YY HH:MM:SS
        char* timestamp();
};
#endif
