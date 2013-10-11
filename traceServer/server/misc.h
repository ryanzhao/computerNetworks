//----------------------------------------------------------------------------
// Some of the helper functions such as signal handlers, automatic timing-out,
// and debug (VOMIT) macros, etc.
//========================================
// Weiran Zhao, Computer Science Dept
// Indiana University, Bloomington
//========================================
// Started: Wed,Oct 02th 2013 03:45:43 PM EDT
// Last Modified: Thu,Oct 10th 2013 03:55:16 PM EDT
//----------------------------------------------------------------------------
#ifndef _MISC_H_
#define _MISC_H_
// add commonly used include files
#include<iostream>
#include<cstdio>
#include<cstdlib>
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
#endif
