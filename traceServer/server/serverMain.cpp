//----------------------------------------------------------------------------
//
//------------
// Reference: 
//------------
//  1.  Beej's guide to network programming
//      http://beej.us/guide/bgnet/output/html/multipage/clientserver.html#simpleserver
//      On how to setup signal handler and reap zombies
//========================================
// Weiran Zhao, Computer Science Dept
// Indiana University, Bloomington
//========================================
// Started: before Tue,Sep 17th 2013 09:39:19 AM EDT
// Modified: Wed,Oct 02th 2013 03:57:10 PM EDT 
//           Handle sigchld
// Modified: Thu,Oct 03th 2013 10:23:02 AM EDT
//           Use C standard library to handle input/output with network
//           connection (aka fdopen, fgets, fputs)
// Modified: Wed,Oct 09th 2013 09:08:09 PM EDT
//           Add handle for automatic timeout of client if no input for 30sec
// Modified: Fri,Oct 11th 2013 11:47:43 AM EDT
//           Finish client server interaction
// Modified: Sun,Oct 13th 2013 03:16:19 PM EDT
//           Add handles for concurrent users 
// Modified: Sun,Oct 13th 2013 04:42:36 PM EDT
//           Add handling for client close connection instead of type 'quit'
// Last Modified: Sun,Oct 13th 2013 04:43:40 PM EDT
//----------------------------------------------------------------------------
#include"progArgs.h"
#include"lib/syscallWrap.h"
#include"misc.h"
#include"eventsLog.h"
#include<cstdlib>
#include<cstdio>
#include<iostream>
#include<cstring>
#include<arpa/inet.h>
#include<unistd.h>
#include<sys/select.h>
#include<sys/time.h>
#include<sys/wait.h>
#include"interaction.h"
#include"rateLimiting.h"

// start server log 
eventsLog servLog;
// current number of users
int numUsers=0;

int main(int argc, char** argv) {
    //-------------------------------------------
    // parse input argument and store it in pArgs
    //-------------------------------------------
    progArgs pArgs(argc,argv);

    //-----------------------------------------------------
    // prepare server's listening sockets and starts listen
    //-----------------------------------------------------
    int listenfd, connfd;
    socklen_t clilen;
    struct sockaddr_in servaddr, cliaddr;
    // create listening socket
    listenfd = Socket(AF_INET, SOCK_STREAM, 0);
    // prepare servaddr for bind
    memset(&servaddr, sizeof(servaddr), 0);
    servaddr.sin_family = AF_INET;
    // bind to any address available
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(pArgs.port);
    // bind
    Bind(listenfd, (SA *) & servaddr, sizeof(servaddr));
    // listen
    Listen(listenfd, pArgs.listenQ);

    //----------------
    // signal handlers
    //----------------
    Signal(SIGCHLD, sigchld_listen_handler);
    Signal(SIGINT,sigint_listen_handler);
    //----------------------------------
    // seek chance to accept from client
    //----------------------------------
    pid_t chd_pid;
    while(1) {
        //----------------------
        // accept the connection
        //----------------------
        clilen = sizeof(cliaddr);
        // clilen is value-result parameters
        connfd = Accept(listenfd, (SA *) &cliaddr, &clilen);

        //------------------------------------------
        // check if we are over max concurrent users
        //------------------------------------------
        if(++numUsers>pArgs.maxUsers) {
            // notify the user and refuse the connection
            dprintf(connfd, "-------------------------------------------"
                    "---------------------------------\n");
            dprintf(connfd, "We have reached max concurrent users for this"
                    " server, which was set to %d\n", pArgs.maxUsers); 
            dprintf(connfd, "Refusing this connection...\n");
            dprintf(connfd, "-------------------------------------------"
                    "---------------------------------\n");
            Close(connfd);
            // decrease num users
            numUsers--;
            // log this event
            servLog.logIt("User from IP: %s get refused because we reached"
                    " max concurrent users\n", userIp(cliaddr));
            // continue
            continue;
        }

        //-----------
        // debug info
        //-----------
        if(VOMIT) {
            cout<<"get connection from: "<<userIp(cliaddr)<<endl;
        }

        //--------------------------------------
        // fork a child to handle client's input
        //--------------------------------------
        if((chd_pid=fork())< 0) {  
            sys_err("call to fork() failed");
        } else if(chd_pid==0) {     // child process
            // every connfd has a 'rate-limitor'
            rateLimiting rateLimitor(pArgs.reqPerSec);
            // restore sigint signal handler
            signal(SIGINT, SIG_DFL);
            signal(SIGCHLD, SIG_DFL);
            // close listening file descriptor inherited from parent
            Close(listenfd);
            //---------------------------------------------
            // set automatic timeout if no input were found
            //---------------------------------------------
            int timerStat;
            while((timerStat=setReadTimer(connfd, pArgs.timeOut))>0) {
                //-----------------------------------
                // handles all kinds of client inputs
                //-----------------------------------
                interaction inter(connfd, &cliaddr, &rateLimitor, pArgs.strictDest);
                FILE* fConn = Fdopen(connfd, "r+");
                // get input from client
                if(fgets(inter.getInBuff(),inter.BUFSIZE, fConn)!=NULL) {
                    // handle input
                    inter.takeAction();
                } else if(feof(fConn)!=0) { // user close connection instead of typing 'quit'
                    // log this event
                    servLog.logIt("User from IP: %s close connection"
                            " instead of typing 'quit'\n", userIp(cliaddr));
                    Close(connfd);
                    exit(0);
                } else {
                    sys_err("call to fgets() failed");
                }
            }
            // error checking on setReadTimer (aka select())
            if(timerStat==-1) {
                Close(connfd);
                sys_err("call to select() failed");
            } else { // automatic time out happened
                dprintf(connfd,"Automatic time out after no input of %d sec\n",pArgs.timeOut);
                // log automatic time out
                servLog.logIt("Automatic time out from %s (with pid: %d),"
                        " after no input of %d seconds\n",
                        userIp(cliaddr), getpid(), pArgs.timeOut);
                Close(connfd);
                exit(0);
            } // timerStat
        } else {    // parent process
            //-----------
            // debug info
            //-----------
            if(VOMIT) {
                cout<<"listening process: "<<getpid()
                    <<" forked a child with pid: "<<chd_pid<<endl;
            }
            // close connfd
            Close(connfd);
            //------------------------------------------------------------------
            // listeing process goes back to the begining of this while loop 
            // and accept next client
            //------------------------------------------------------------------
        } // child_pid
    }
    //------------------------------------------------------------------
    // should never run to here, instead should exit from sigint handler
    //------------------------------------------------------------------
    return -100;
}
