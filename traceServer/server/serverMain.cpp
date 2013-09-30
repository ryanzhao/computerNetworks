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
// Last Modified: Sun,Sep 29th 2013 11:36:03 PM EDT
//----------------------------------------------------------------------------
#include"progArgs.h"
#include"eventsLog.h"
#include"lib/syscallWrap.h"
#include<cstdlib>
#include<iostream>
#include<cstring>
#include<arpa/inet.h>
#include<unistd.h>
#include<sys/select.h>
#include<sys/time.h>
#include<sys/wait.h>

void sigchld_handler(int sig) {
    pid_t pid;
    pid = waitpid(-1, NULL, WNOHANG);
    cout<<"Child with pid: "<<pid<<" terminated"<<endl;
}

using namespace std;
#define VOMIT 1

int main(int argc, char** argv) {
    //-------------------------------------------
    // parse input argument and store it in pArgs
    //-------------------------------------------
    progArgs pArgs(argc,argv);
    //-----------------
    // start server log
    //-----------------
    eventsLog servLog;

    //-----------------------------------------------------
    // prepare server's listening sockets and starts listen
    //-----------------------------------------------------
    int listenfd, connfd;
    socklen_t clilen;
    struct sockaddr_in servaddr, cliaddr;
    // creat listening socket
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

    //----------------------------------
    // seek chance to accept from client
    //----------------------------------
    pid_t chd_pid;
    // set up signal handler for SIGCHLD
    Signal(SIGCHLD, sigchld_handler);
    while(1) {
        // accept
        clilen = sizeof(cliaddr);
        // clilen is value-result parameters
        connfd = Accept(listenfd, (SA *) &cliaddr, &clilen);
        if(VOMIT) {
            char str[INET_ADDRSTRLEN];
            cout<<"get connection from: "<<inet_ntop(AF_INET, &cliaddr.sin_addr, str, sizeof(str))<<endl;
        }
        if((chd_pid=fork())< 0) {  
            sys_err("call to fork() failed");
        } else if(chd_pid==0) {     // child process
            //---------------------------------------------
            // set automatic timeout if no input were found
            //---------------------------------------------
            int maxfdp1 = connfd+1;
            fd_set readSet;
            FD_SET(connfd, &readSet);
            struct timeval waittime;
            waittime.tv_sec=5;
            waittime.tv_usec=0;
            if( select(maxfdp1,&readSet, NULL, NULL, &waittime) ==0) {
                const char* output="time out\n";
                write(connfd,output, strlen(output));
                Close(listenfd);
                exit(0);
            }
            else {
                const char* output="some input\n";
                write(connfd,output, strlen(output));
                Close(listenfd);
                exit(0);
            }
        } else {    // parent process
            cout<<"forked a child with pid: "<<chd_pid<<endl;
        }
    }
    Close(connfd);

    return 0;
}
