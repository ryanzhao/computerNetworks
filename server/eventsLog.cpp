//----------------------------------------------------------------------------
// Check "eventsLog.h" for details
//========================================
// Weiran Zhao, Computer Science Dept
// Indiana University, Bloomington
//========================================
// Started: before Tue,Sep 17th 2013 05:44:55 PM EDT
// Modified: Thu,Oct 10th 2013 11:56:08 PM EDT
//           change 'log.txt' access mode to append
// Modified: Fri,Oct 11th 2013 12:16:09 PM EDT
//           Modify logIt() function, let it take arguments of variable length
//           Also has to change to use FILE stream instead of ofstream
// Last Modified: Sun,Oct 13th 2013 03:49:18 PM EDT
//----------------------------------------------------------------------------
#include"eventsLog.h"
eventsLog::eventsLog(){
    ofile = fopen("log.txt","a");
    // have to set this FILE stream line buffered or non-buffered
//    setvbuf(ofile,NULL,_IOLBF,BUFSIZ);
    setvbuf(ofile,NULL,_IOLBF,0);
    const char* delim = "------------------------------------------------"
                        "----------------------------";
    fprintf(ofile,"%s\n",delim);
    logIt("Server started!\n");
}

eventsLog::~eventsLog() {
    fclose(ofile);
}

void eventsLog::logIt(const char* format,...) {
    if(ofile!=NULL) {
        fprintf(ofile,"%s",timestamp());
        va_list args;
        va_start(args, format);
        vfprintf(ofile, format, args);
        va_end(args);
    } else {
        fprintf(stderr,"File 'log.txt' not open\n");
    }
}

char* eventsLog::timestamp() {
    memset(timeStampBuff,0,20);
    time_t raw_t=time(NULL);
    struct tm* timeInfo = localtime(&raw_t);
    strftime(timeStampBuff,20,"%D %T ",timeInfo);
    return timeStampBuff;
}
