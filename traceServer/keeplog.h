//
//  keeplog.h
//  dtraceroute
//
//  Created by Weiran Zhao on 9/24/12.
//  Copyright (c) 2012 Weiran Zhao. All rights reserved.
//

#ifndef dtraceroute_keeplog_h
#define dtraceroute_keeplog_h

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <time.h>

#include "utils.h"

//  name of the log file
#define LOGNAME     "log"

//  used to generate time stamp of the format MM/DD/YY/HH:MM:SS
void timestamp(char* tmstamp);

//  write message to log
void writelog(char* msg);
#endif
