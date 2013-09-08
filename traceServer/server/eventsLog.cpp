#include"eventsLog.h"
#include<iostream>
#include<ctime>
eventsLog::eventsLog(){
    outfile.open("log.txt");
    if(outfile.is_open()) {
        outfile<<timestamp()<<"Server Started!"<<endl;
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
