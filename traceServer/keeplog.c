//For detailed functionality, see comments in "keeplog.h"

#include "keeplog.h"

void timestamp(char* tmstamp)
{
    if (tmstamp==NULL)
    {
        printf("need to allocate space for tmstamp first\n");
        exit(0);
    }
    time_t now;
    time(&now);
    struct tm tminfo= *(localtime(&now));
    sprintf(tmstamp,
            "%02d/%02d/%02d/%02d:%02d:%02d",    //time format
            tminfo.tm_mon+1,                    //month
            tminfo.tm_mday,                     //day
            (tminfo.tm_year+1900)%100,          //year
            tminfo.tm_hour,                     //hour
            tminfo.tm_min,                      //minute
            tminfo.tm_sec);                     //seconds
}

void writelog(char* msg)
{
    FILE* fp=fopen(LOGNAME, "a");
    char tmstamp[BUFSIZE];
    timestamp(tmstamp);
    fprintf(fp, "%s \t PID %d writes: %s",tmstamp,getpid(),msg);
    fclose(fp);
//    printf("PID %d writes: %s",getpid(), msg);
}
