//
//  utils.h
//  dtraceroute
//
//  Created by Weiran Zhao on 9/24/12.
//  Copyright (c) 2012 Weiran Zhao. All rights reserved.
//

#ifndef dtraceroute_utils_h
#define dtraceroute_utils_h

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

#include "keeplog.h"

#define BUFSIZE     1024        //read from client input
#define MAXLINELEN  1024        //maximun line length

#define QUIT        0           //quit
#define HELP        1           //help
#define TRACME      2           //traceroute me
#define TRACFL      3           //traceroute from file
#define TRACNM      4           //regular traceroute
#define TRACSC      5           //trace with source ip options
#define UNKN        6           //unknown input

//-------------------------------------
//  variables controlling behavior of
//  the traceroute_sever
//-------------------------------------
char port[10];
int users;
int requests;
int interval;
int strict;
int backlog;
struct timeval rcv_timeout;

//--------------------------------------
// things for rate control
//--------------------------------------
#define RATEOK      1   //rate ok
#define OVERRATE    -1  //over rate limiting

struct _raterecord
{
    int maxreq;         //maximum number of requests
    int interval;       //per interval seconds
    int reqidx;         //request index
    int* reqlist;       //a list of request's time
};

typedef struct _raterecord raterecord;

//used to initilize rate record
raterecord* rrinit();

//free raterecords
void freerecord(raterecord* rr);

// do rate-limiting
int ratelimit(raterecord* rr);


//--------------------------------------
// a bunch of useful functions
//--------------------------------------

//signal used to kill zombies
void sigchld_handler(int s);

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa);

//give instruction on usage of the traceroute_server utility
void usage();

//deal with client quits
void clientquit(int new_fd);

//print help message during a tcp connection with traceroute_server
void printhelp(int new_fd);

//this deals with client input invalid arguments
void invalidargs(int new_fd);

//traceroute client machine
void traceme(int new_fd, char* client_address, raterecord* rr);

//traceroute based on file input
void tracefile(int new_fd, char* fname, raterecord* rr);

//traceroute with -s src_ip option
void tracesrcip(int new_fd, char* src_ip, char* client_address, int strict, char* client_input, raterecord* rr);

//a regular traceroute
void tracenormal(int new_fd, char* client_input, raterecord* rr);

//get the task type (quit, help, traceroute or etc) by analyze client's input
int gettasktype(char* client_input, char* fname, char* src_ip);

//set default value of variables that control server's behavior
void setdfltvals();

//------------------------------------
//  concurrent related
//------------------------------------
//reopen a slot after a client connection is closed
void limit_handler(int sig);

//notify client for maximum user reached
void limit_notify(int new_fd, int allowable_users);

//-------------------------------------
// deals with timeout
//-------------------------------------
//prints out timeout messages
void tomessage(int new_fd);


#endif
