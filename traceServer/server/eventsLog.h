//----------------------------------------------------------------------------
// A log class keep track of traceServer activity and log abnormal client usage
// or requests.
//========================================
// Weiran Zhao, Computer Science Dept
// Indiana University, Bloomington
//========================================
// Started: a week or so before Tue,Sep 17th 2013 05:46:20 PM EDT
// Last Modified: Tue,Sep 17th 2013 05:46:31 PM EDT
//----------------------------------------------------------------------------
#ifndef EVENTS_LOG_H_
#define EVENTS_LOG_H_
#include<fstream>
#include<string>
using namespace std;
class eventsLog {
    private:
        ofstream outfile;
    public:
        eventsLog();
        ~eventsLog();
        // log a message into 'log.txt'
        void logIt(const char* msg);
        // return a timestamp of format MM/DD/YY HH:MM:SS
        string timestamp();
};
#endif
