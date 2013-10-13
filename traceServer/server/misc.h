//----------------------------------------------------------------------------
// Some of the helper functions such as signal handlers, automatic timing-out,
// and debug (VOMIT) macros, etc.
//========================================
// Weiran Zhao, Computer Science Dept
// Indiana University, Bloomington
//========================================
// Started: Wed,Oct 02th 2013 03:45:43 PM EDT
// Modified: Sun,Oct 13th 2013 04:30:53 PM EDT
//           Add userIp() function
// Last Modified: Sun,Oct 13th 2013 04:31:33 PM EDT
//----------------------------------------------------------------------------
#ifndef _MISC_H_
#define _MISC_H_
// add commonly used include files
#include<iostream>
#include<cstdio>
#include<cstdlib>
#include"eventsLog.h"
#include<arpa/inet.h>
#include<cstring>
using namespace std;

// temp buffer size, mostly for char
#define TEMPBUFFSIZE 1024
// dubug info. switch
#define VOMIT 1
//-------------------------------------------------------------------------
// set read timer to 'sec' seconds and check if there's input from 'connfd'
// if not (meaning time out) return 0; return positive number if there's 
// input; otherwise return -1 for error
//-------------------------------------------------------------------------
int setReadTimer(int fd, int sec);
//----------------------------------
// signal handler for listen process
//----------------------------------
void sigchld_listen_handler(int sig);
void sigint_listen_handler(int sig);
//--------------------------------------
// return a string consisting of user ip
//--------------------------------------
char* userIp(struct sockaddr_in cliaddr);
#endif
