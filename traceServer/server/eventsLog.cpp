//----------------------------------------------------------------------------
// Check "eventsLog.h" for details
//========================================
// Weiran Zhao, Computer Science Dept
// Indiana University, Bloomington
//========================================
// Started: before Tue,Sep 17th 2013 05:44:55 PM EDT
// Modified: Thu,Oct 10th 2013 11:56:08 PM EDT
//           change 'log.txt' access mode to append
// Last Modified: Fri,Oct 11th 2013 12:11:10 PM EDT
//----------------------------------------------------------------------------
#include"eventsLog.h"
#include<iostream>
#include<ctime>
eventsLog::eventsLog(){
    outfile.open("log.txt", ofstream::app);
    if(outfile.is_open()) {
        outfile<<"-----------------------------------------------"
            <<"-----------------------------"<<endl;
        outfile<<timestamp()<<"Server started!"<<endl;
    } else {
        cerr<<"File 'log.txt' open failed"<<endl;
    }
}

eventsLog::~eventsLog() {
    outfile.close();
}

void eventsLog::logIt(const char* msg) {
    if(outfile.is_open()) {
        outfile<<timestamp()<<msg<<endl;
    } else {
        cerr<<"File 'log.txt' not open"<<endl;
    }
}

string eventsLog::timestamp() {
    char buffer[20];
    time_t raw_t=time(NULL);
    struct tm* timeInfo = localtime(&raw_t);
    strftime(buffer,20,"%D %T ",timeInfo);
    return string(buffer);
}
